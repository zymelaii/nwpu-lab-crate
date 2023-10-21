/*
 * Pseudo-fat filesystem.
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.54 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <limits.h>
#include <geekos/errno.h>
#include <geekos/screen.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/ide.h>
#include <geekos/blockdev.h>
#include <geekos/bitset.h>
#include <geekos/vfs.h>
#include <geekos/list.h>
#include <geekos/synch.h>
#include <geekos/pfat.h>

/*
 * History:
 * 13-Nov-2003: Converted to use new block device API.
 * 17-Dec-2003: Rewrite to conform to new VFS layer
 * 19-Feb-2004: Cache and share PFAT_File objects, instead of
 *   allocating them repeatedly
 */

/*
 * TODO:
 * - Support hierarchical directories
 */

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

#define PAGEFILE_FILENAME "/pagefile.bin"

int debugPFAT = 0;
#define Debug(args...) if (debugPFAT) Print("PFAT: " args)

struct PFAT_File;
DEFINE_LIST(PFAT_File_List, PFAT_File);

/*
 * In-memory information describing a mounted PFAT filesystem.
 * This is kept in the fsInfo field of the Mount_Point.
 */
struct PFAT_Instance {
    bootSector fsinfo;
    int *fat;
    directoryEntry *rootDir;
    directoryEntry rootDirEntry;
    struct Mutex lock;
    struct PFAT_File_List fileList;
};

/*
 * In-memory information for a particular open file.
 * In particular, this object contains a cache of the contents
 * of the file.
 * Kept in fsInfo field of File.
 */
struct PFAT_File {
    directoryEntry *entry;		 /* Directory entry of the file */
    ulong_t numBlocks;			 /* Number of blocks used by file */
    char *fileDataCache;		 /* File data cache */
    struct Bit_Set *validBlockSet;	 /* Which data blocks of cache are valid */
    struct Mutex lock;			 /* Synchronize concurrent accesses */
    DEFINE_LINK(PFAT_File_List, PFAT_File);
};
IMPLEMENT_LIST(PFAT_File_List, PFAT_File);

/*
 * Copy file metadata from directory entry into
 * struct VFS_File_Stat object.
 */
static void Copy_Stat(struct VFS_File_Stat *stat, directoryEntry *entry)
{
    stat->size = entry->fileSize;
    stat->isDirectory = entry->directory;

    stat->isSetuid = 0;
    memset(&stat->acls, '\0', sizeof(stat->acls));
    stat->acls[0].uid = 0;
    stat->acls[0].permission = O_READ;
    if (!entry->readOnly)
	stat->acls[0].permission |= O_WRITE;
}

/*
 * FStat function for PFAT files.
 */
static int PFAT_FStat(struct File *file, struct VFS_File_Stat *stat)
{
    struct PFAT_File *pfatFile = (struct PFAT_File*) file->fsData;
    Copy_Stat(stat, pfatFile->entry);
    return 0;
}

/*
 * Read function for PFAT files.
 */
static int PFAT_Read(struct File *file, void *buf, ulong_t numBytes)
{
    struct PFAT_File *pfatFile = (struct PFAT_File*) file->fsData;
    struct PFAT_Instance *instance = (struct PFAT_Instance*) file->mountPoint->fsData;
    ulong_t start = file->filePos;
    ulong_t end = file->filePos + numBytes;
    ulong_t startBlock, endBlock, curBlock;
    ulong_t i;

    /* Special case: can't handle reads longer than INT_MAX */
    if (numBytes > INT_MAX)
	return EINVALID;

    /* Make sure request represents a valid range within the file */
    if (start >= file->endPos || end > file->endPos || end < start) {
	Debug("Invalid read position: filePos=%lu, numBytes=%lu, endPos=%lu\n",
	    file->filePos, numBytes, file->endPos);
	return EINVALID;
    }

    /*
     * Now the complicated part; ensure that all blocks containing the
     * data we need are in the file data cache.
     */
    startBlock = (start % SECTOR_SIZE) / SECTOR_SIZE;
    endBlock = Round_Up_To_Block(end) / SECTOR_SIZE;

    /*
     * Traverse the FAT finding the blocks of the file.
     * As we encounter requested blocks that aren't in the
     * file data cache, we issue requests to read them.
     */
    curBlock = pfatFile->entry->firstBlock;
    for (i = 0; i < endBlock; ++i) {
	/* Are we at a valid block? */
	if (curBlock == FAT_ENTRY_FREE || curBlock == FAT_ENTRY_EOF) {
	    Print("Unexpected end of file in FAT at file block %lu\n", i);
	    return EIO;  /* probable filesystem corruption */
	}

	/* Do we need to read this block? */
	if (i >= startBlock) {
	    int rc = 0;

	    /* Only allow one thread at a time to read this block. */
	    Mutex_Lock(&pfatFile->lock);

	    if (!Is_Bit_Set(pfatFile->validBlockSet, i)) {
		/* Read block into the file data cache */
		Debug("Reading file block %lu (device block %lu)\n", i, curBlock);
		rc = Block_Read(file->mountPoint->dev, curBlock, pfatFile->fileDataCache + i*SECTOR_SIZE);

		if (rc == 0)
		    /* Mark as having read this block */
		    Set_Bit(pfatFile->validBlockSet, i);
	    }

	    /* Done attempting to fetch the block */
	    Mutex_Unlock(&pfatFile->lock);

	    if (rc != 0)
		return rc;
	}

	/* Continue to next block */
	ulong_t nextBlock = instance->fat[curBlock];
	curBlock = nextBlock;
    }

    /*
     * All cached data we need is up to date,
     * so just copy it into the caller's buffer.
     */
    memcpy(buf, pfatFile->fileDataCache + start, numBytes);

    Debug("Read satisfied!\n");

    return numBytes;
}

/*
 * Write function for PFAT files.
 */
static int PFAT_Write(struct File *file, void *buf, ulong_t numBytes)
{
    /* Read only fs: writes not allowed */
    return EACCESS;
}

/*
 * Seek function for PFAT files.
 */
static int PFAT_Seek(struct File *file, ulong_t pos)
{
    if (pos >= file->endPos)
	return EINVALID;
     file->filePos = pos;
     return 0;
}

/*
 * Close function for PFAT files.
 */
static int PFAT_Close(struct File *file)
{
    /*
     * The PFAT_File object caching the contents of the file
     * will remain in the PFAT_Instance object, to speed up
     * future accesses to this file.
     */
    return 0;
}

/*
 * File_Ops for PFAT files.
 */
static struct File_Ops s_pfatFileOps = {
    &PFAT_FStat,
    &PFAT_Read,
    &PFAT_Write,
    &PFAT_Seek,
    &PFAT_Close,
    0, /* Read_Entry */
};

static int PFAT_FStat_Dir(struct File *dir, struct VFS_File_Stat *stat)
{
    /* FIXME: for now, there is only one directory */
    struct PFAT_Instance *instance = (struct PFAT_Instance*) dir->mountPoint->fsData;
    Copy_Stat(stat, &instance->rootDirEntry);
    return 0;
}

/*
 * Close function for PFAT directories.
 */
static int PFAT_Close_Dir(struct File *dir)
{
    /* This is a no-op. */
    return 0;
}

/*
 * Read a directory entry.
 */
static int PFAT_Read_Entry(struct File *dir, struct VFS_Dir_Entry *entry)
{
    directoryEntry *pfatDirEntry;
    struct PFAT_Instance *instance = (struct PFAT_Instance*) dir->mountPoint->fsData;

    if (dir->filePos >= dir->endPos)
	return VFS_NO_MORE_DIR_ENTRIES; /* Reached the end of the directory. */

    pfatDirEntry = &instance->rootDir[dir->filePos++];

    /*
     * Note: we don't need to bounds check here, because
     * generic struct VFS_Dir_Entry objects have much more space for filenames
     * than PFAT directoryEntry objects.
     */
    strncpy(entry->name, pfatDirEntry->fileName, sizeof(pfatDirEntry->fileName));
    entry->name[sizeof(pfatDirEntry->fileName)] = '\0';

    Copy_Stat(&entry->stats, pfatDirEntry);

    return 0;
}

/*
 * File_Ops for PFAT directories.
 */
static struct File_Ops s_pfatDirOps = {
    &PFAT_FStat_Dir,
    0, /* Read */
    0, /* Write */
    0, /* Seek */
    &PFAT_Close_Dir,
    &PFAT_Read_Entry,
};

/*
 * Look up a directory entry in a PFAT filesystem.
 */
static directoryEntry *PFAT_Lookup(struct PFAT_Instance *instance, const char *path)
{
    directoryEntry *rootDir = instance->rootDir;
    bootSector *fsinfo = &instance->fsinfo;
    int i;

    KASSERT(*path == '/');

    /* Special case: root directory. */
    if (strcmp(path, "/") == 0)
	return &instance->rootDirEntry;

    /* Skip leading '/' character. */
    ++path;

    /*
     * FIXME: Eventually, we should try to implement hierarchical
     * directory structure.  For now, only the root directory
     * is supported.
     */
    for (i = 0; i < fsinfo->rootDirectoryCount; ++i) {
    	directoryEntry *entry = &rootDir[i];
	if (strcmp(entry->fileName, path) == 0) {
	    /* Found it! */
	    Debug("Found matching dir entry for %s\n", path);
	    return entry;
	}
    }

    /* Not found. */
    return 0;
}

/*
 * Get a PFAT_File object representing the file whose directory entry
 * is given.
 */
static struct PFAT_File *Get_PFAT_File(struct PFAT_Instance *instance, directoryEntry *entry)
{
    ulong_t numBlocks;
    struct PFAT_File *pfatFile = 0;
    char *fileDataCache = 0;
    struct Bit_Set *validBlockSet = 0;

    KASSERT(entry != 0);
    KASSERT(instance != 0);

    Mutex_Lock(&instance->lock);

    /*
     * See if this file has already been opened.
     * If so, use the existing PFAT_File object.
     */
    for (pfatFile = Get_Front_Of_PFAT_File_List(&instance->fileList);
	 pfatFile != 0;
	 pfatFile = Get_Next_In_PFAT_File_List(pfatFile)) {
	if (pfatFile->entry == entry)
	    break;
    }

    if (pfatFile == 0) {
	/* Determine size of data block cache for file. */
	numBlocks = Round_Up_To_Block(entry->fileSize) / SECTOR_SIZE;

	/*
	 * Allocate File object, PFAT_File object, file block data cache,
	 * and valid cache block bitset
	 */
	if ((pfatFile = (struct PFAT_File *) Malloc(sizeof(*pfatFile))) == 0 ||
	    (fileDataCache = Malloc(numBlocks * SECTOR_SIZE)) == 0 ||
	    (validBlockSet = Create_Bit_Set(numBlocks)) == 0) {
	    goto memfail;
	}

	/* Populate PFAT_File */
	pfatFile->entry = entry;
	pfatFile->numBlocks = numBlocks;
	pfatFile->fileDataCache = fileDataCache;
	pfatFile->validBlockSet = validBlockSet;
	Mutex_Init(&pfatFile->lock);

	/* Add to instance's list of PFAT_File objects. */
	Add_To_Back_Of_PFAT_File_List(&instance->fileList, pfatFile);
	KASSERT(pfatFile->nextPFAT_File_List == 0);
    }

    /* Success! */
    goto done;

memfail:
    if (pfatFile != 0)
	Free(pfatFile);
    if (fileDataCache != 0)
	Free(fileDataCache);
    if (validBlockSet != 0)
	Free(validBlockSet);

done:
    Mutex_Unlock(&instance->lock);
    return pfatFile;
}

/*
 * Open function for PFAT filesystems.
 */
static int PFAT_Open(struct Mount_Point *mountPoint, const char *path, int mode, struct File **pFile)
{
    int rc = 0;
    struct PFAT_Instance *instance = (struct PFAT_Instance*) mountPoint->fsData;
    directoryEntry *entry;
    struct PFAT_File *pfatFile = 0;
    struct File *file = 0;

    /* Reject attempts to create or write */
    if ((mode & (O_WRITE | O_CREATE)) != 0)
	return EACCESS;

    /* Look up the directory entry */
    entry = PFAT_Lookup(instance, path);
    if (entry == 0)
	return ENOTFOUND;

    /* Make sure the entry is not a directory. */
    if (entry->directory)
	return EACCESS;

    /* Get PFAT_File object */
    pfatFile = Get_PFAT_File(instance, entry);
    if (pfatFile == 0)
	goto done;

    /* Create the file object. */
    file = Allocate_File(&s_pfatFileOps, 0, entry->fileSize, pfatFile, 0, 0);
    if (file == 0) {
	rc = ENOMEM;
	goto done;
    }

    /* Success! */
    *pFile = file;

done:
    return rc;
}

/*
 * Open_Directory function for PFAT filesystems.
 */
static int PFAT_Open_Directory(struct Mount_Point *mountPoint, const char *path, struct File **pDir)
{
    /*
     * FIXME: for now, we only support a single directory.
     * This makes this function pretty simple.
     * We just store the current cursor index in the File object.
     */
    struct PFAT_Instance *instance = (struct PFAT_Instance*) mountPoint->fsData;
    struct File *dir;

    if (strcmp(path, "/") != 0)
	return ENOTFOUND;

    dir = (struct File*) Malloc(sizeof(*dir));
    if (dir == 0)
	return ENOMEM;

    dir->ops = &s_pfatDirOps;
    dir->filePos = 0; /* next dir entry to be read */
    dir->endPos = instance->fsinfo.rootDirectoryCount; /* number of directory entries */
    dir->fsData = 0;

    *pDir = dir;
    return 0;
}

/*
 * Stat function for PFAT filesystems.
 */
static int PFAT_Stat(struct Mount_Point *mountPoint, const char *path, struct VFS_File_Stat *stat)
{
    struct PFAT_Instance *instance = (struct PFAT_Instance*) mountPoint->fsData;
    directoryEntry *entry;

    KASSERT(path != 0);
    KASSERT(stat != 0);

    Debug("PFAT_Stat(%s)\n", path);

    entry = PFAT_Lookup(instance, path);
    if (entry == 0)
	return ENOTFOUND;

    Copy_Stat(stat, entry);

    return 0;
}

/*
 * Sync function for PFAT filesystems.
 */
static int PFAT_Sync(struct Mount_Point *mountPoint)
{
    /* Read only filesystem: this is a no-op. */
    return 0;
}

/*
 * Mount_Point_Ops for PFAT filesystem.
 */
struct Mount_Point_Ops s_pfatMountPointOps = {
    PFAT_Open,
    0,				/* Create_Directory() */
    PFAT_Open_Directory,
    PFAT_Stat,
    PFAT_Sync,
    0                          /* Delete */
};

/*
 * If the given PFAT instance has a paging file,
 * register it as the paging device, unless a paging device
 * has already been registered.
 */
static void PFAT_Register_Paging_File(struct Mount_Point *mountPoint, struct PFAT_Instance *instance)
{
    directoryEntry *pagefileEntry;
    struct Paging_Device *pagedev = 0;
    size_t nameLen;
    char *fileName = 0;

    if (Get_Paging_Device() != 0)
	return;  /* A paging device is already registered */

    pagefileEntry = PFAT_Lookup(instance, PAGEFILE_FILENAME);
    if (pagefileEntry == 0)
	return;  /* No paging file in this filesystem */

    /* TODO: verify that paging file is contiguous */

    /* Create Paging_Device object. */
    pagedev = (struct Paging_Device*) Malloc(sizeof(*pagedev));
    if (pagedev == 0)
	goto memfail;
    nameLen = strlen(mountPoint->pathPrefix) + strlen(PAGEFILE_FILENAME) + 3;
    fileName = (char*) Malloc(nameLen);
    if (fileName == 0)
	goto memfail;

    /* Format page filename */
    snprintf(fileName, nameLen, "/%s%s", mountPoint->pathPrefix, PAGEFILE_FILENAME);

    /* Initialize Paging_Device */
    pagedev->fileName = fileName;
    pagedev->dev = mountPoint->dev;
    pagedev->startSector = pagefileEntry->firstBlock;
    pagedev->numSectors = pagefileEntry->fileSize / SECTOR_SIZE;

    /* Register it */
    Register_Paging_Device(pagedev);
    return;

memfail:
    Print("  Error: could not create paging device for pfat on %s (%s)\n",
	mountPoint->pathPrefix, mountPoint->dev->name);
    if (pagedev != 0)
	Free(pagedev);
    if (fileName != 0)
	Free(fileName);
}

/*
 * Mount function for PFAT filesystem.
 */
static int PFAT_Mount(struct Mount_Point *mountPoint)
{
    struct PFAT_Instance *instance = 0;
    bootSector *fsinfo;
    void *bootSect = 0;
    int rootDirSize;
    int rc;
    int i;

    /* Allocate instance. */
    instance = (struct PFAT_Instance*) Malloc(sizeof(*instance));
    if (instance == 0)
	goto memfail;
    memset(instance, '\0', sizeof(*instance));
    fsinfo = &instance->fsinfo;
    Debug("Created instance object\n");

    /*
     * Allocate buffer to read bootsector,
     * which contains metainformation about the PFAT filesystem.
     */
    bootSect = Malloc(SECTOR_SIZE);
    if (bootSect == 0)
	goto memfail;

    /* Read boot sector */
    if ((rc = Block_Read(mountPoint->dev, 0, bootSect)) < 0)
	goto fail;
    Debug("Read boot sector\n");

    /* Copy filesystem parameters from boot sector */
    memcpy(&instance->fsinfo, ((char*)bootSect) + PFAT_BOOT_RECORD_OFFSET, sizeof(bootSector));
    Debug("Copied boot record\n");

    /* Does magic number match? */
    if (fsinfo->magic != PFAT_MAGIC) {
	Print("Bad magic number (%x) for PFAT filesystem\n", fsinfo->magic);
	goto invalidfs;
    }
    Debug("Magic number is good!\n");

    /* Do filesystem params look reasonable? */
    if (fsinfo->fileAllocationOffset <= 0 ||
	fsinfo->fileAllocationLength <= 0 ||
	fsinfo->rootDirectoryCount < 0 ||
	fsinfo->rootDirectoryOffset <= 0) {
	Print("Invalid parameters for PFAT filesystem\n");
	goto invalidfs;
    }
    Debug("PFAT filesystem parameters appear to be good!\n");

    /* Allocate in-memory FAT */
    instance->fat = (int*) Malloc(fsinfo->fileAllocationLength * SECTOR_SIZE);
    if (instance->fat == 0)
	goto memfail;

    /* Read the FAT */
    for (i = 0; i < fsinfo->fileAllocationLength; ++i) {
	int blockNum = fsinfo->fileAllocationOffset + i;
	char *p = ((char*)instance->fat) + (i * SECTOR_SIZE);
	if ((rc = Block_Read(mountPoint->dev, blockNum, p)) < 0)
	    goto fail;
    }
    Debug("Read FAT successfully!\n");

    /* Allocate root directory */
    rootDirSize = Round_Up_To_Block(sizeof(directoryEntry) * fsinfo->rootDirectoryCount);
    instance->rootDir = (directoryEntry*) Malloc(rootDirSize);

    /* Read the root directory */
    Debug("Root directory size = %d\n", rootDirSize);
    for (i = 0; i < rootDirSize; i += SECTOR_SIZE) {
	int blockNum = fsinfo->rootDirectoryOffset + i;
	if ((rc = Block_Read(mountPoint->dev, blockNum, instance->rootDir + (i*SECTOR_SIZE))) < 0)
	    goto fail;
    }
    Debug("Read root directory successfully!\n");

    /* Create the fake root directory entry. */
    memset(&instance->rootDirEntry, '\0', sizeof(directoryEntry));
    instance->rootDirEntry.readOnly = 1;
    instance->rootDirEntry.directory = 1;
    instance->rootDirEntry.fileSize =
	instance->fsinfo.rootDirectoryCount * sizeof(directoryEntry);

    /* Initialize instance lock and PFAT_File list. */
    Mutex_Init(&instance->lock);
    Clear_PFAT_File_List(&instance->fileList);

    /* Attempt to register a paging file */
    PFAT_Register_Paging_File(mountPoint, instance);

    /*
     * Success!
     * This mount point is now ready
     * to handle file accesses.
     */
    mountPoint->ops = &s_pfatMountPointOps;
    mountPoint->fsData = instance;
    return 0;

memfail:
    rc = ENOMEM; goto fail;
invalidfs:
    rc = EINVALIDFS; goto fail;
fail:
    if (instance != 0) {
	if (instance->fat != 0)
	    Free(instance->fat);
	if (instance->rootDir != 0)
	    Free(instance->rootDir);
	Free(instance);
    }
    if (bootSect != 0)
	Free(bootSect);
    return rc;
}

static struct Filesystem_Ops s_pfatFilesystemOps = {
    0, // Format
    &PFAT_Mount,
};

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

void Init_PFAT(void)
{
    Register_Filesystem("pfat", &s_pfatFilesystemOps);
}
