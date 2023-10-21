/*
 * Virtual filesystem structures and routines
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.46 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/list.h>
#include <geekos/string.h>
#include <geekos/screen.h>
#include <geekos/malloc.h>
#include <geekos/synch.h>
#include <geekos/vfs.h>

/*
 * Notes:
 * - This is rewritten from Jeff's original VFS implementation.
 *   It uses vtables to more easily accomodate multiple filesystem types.
 */

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * We use a single mutex to protect VFS data structures
 * from concurrent access/modification.  Simple, but not efficient.
 * Perhaps this should really be a reader/writer lock.
 */
static struct Mutex s_vfsLock;

int debugVFS = 0;
#define Debug(args...) if (debugVFS) Print("VFS: " args)

struct Filesystem;

DEFINE_LIST(Mount_Point_List, Mount_Point);
IMPLEMENT_LIST(Mount_Point_List, Mount_Point);

/* List of mounted filesystems. */
static struct Mount_Point_List s_mountPointList;

/* A registered filesystem type. */
struct Filesystem {
    struct Filesystem_Ops *ops;
    char fsName[VFS_MAX_FS_NAME_LEN + 1];
    DEFINE_LINK(Filesystem_List, Filesystem);
};

DEFINE_LIST(Filesystem_List, Filesystem);
IMPLEMENT_LIST(Filesystem_List, Filesystem);

/* List of registered filesystem types. */
static struct Filesystem_List s_filesystemList;

/* Registered paging device. */
static struct Paging_Device *s_pagingDevice;

#define MAX_PREFIX_LEN 16

/*
 * Unpack a path into prefix and suffix.
 * The prefix determines which mounted filesystem
 * the path resides in.  The suffix is the path within
 * the filesystem.
 * Params:
 *   path - the complete path
 *   prefix - buffer where prefix will be stored
 *   pSuffix - stores the pointer to the suffix part of path
 * Returns: true if path is valid, false if not
 */
static bool Unpack_Path(const char *path, char *prefix, const char **pSuffix)
{
    char *slash;
    size_t pfxLen;

    Debug("path=%s\n", path);

    /* Path must start with '/' */
    if (*path != '/')
	return false;
    ++path;

    /* Look for the initial slash. */
    slash = strchr(path, '/');
    if (slash == 0) {
	/*
	 * Special case: path of the form "/prefix".
	 * It resolves to the root directory of
	 * the filesystem mounted on the prefix.
	 */
	pfxLen = strlen(path);
	if (pfxLen == 0 || pfxLen > MAX_PREFIX_LEN)
	    return false;
	strcpy(prefix, path);
	*pSuffix = "/";
    } else {
	/*
	 * Determine length of file prefix.
	 * It needs to be non-zero, but less than MAX_PREFIX_LEN.
	 */
	pfxLen = slash - path;
	if (pfxLen == 0 || pfxLen > MAX_PREFIX_LEN)
	    return false;

	/* Format the path prefix as a string */
	memcpy(prefix, path, pfxLen);
	prefix[pfxLen] = '\0';

	/*
	 * Set pointer to "suffix", i.e., the rest of the path
	 * after the prefix
	 */
	*pSuffix = slash;
    }

    Debug("prefix=%s, suffix=%s\n", prefix, *pSuffix);
    KASSERT(**pSuffix == '/');

    return true;
}

/*
 * Look up given filesystem type.
 * Params:
 *   fstype - name of the filesystem type
 * Returns: the Filesystem object for the filesystem, or null
 *   if no such filesystem exists.
 */
static struct Filesystem *Lookup_Filesystem(const char *fstype)
{
    struct Filesystem *fs;

    Mutex_Lock(&s_vfsLock);
    fs = Get_Front_Of_Filesystem_List(&s_filesystemList);
    while (fs != 0) {
	if (strcmp(fs->fsName, fstype) == 0)
	    break;
	fs = Get_Next_In_Filesystem_List(fs);
    }
    Mutex_Unlock(&s_vfsLock);

    return fs;
}

/*
 * Look up mount point for given prefix.
 *   prefix - the path prefix
 * Returns: the mount point, or null if there is no mount point
 *   matching the prefix
 */
static struct Mount_Point *Lookup_Mount_Point(const char *prefix)
{
    struct Mount_Point *mountPoint;

    Mutex_Lock(&s_vfsLock);

    /* Look for a mounted filesystem with a matching prefix */
    mountPoint = Get_Front_Of_Mount_Point_List(&s_mountPointList);
    while (mountPoint != 0) {
	Debug("Lookup mount point: %s,%s\n", prefix, mountPoint->pathPrefix);
	if (strcmp(prefix, mountPoint->pathPrefix) == 0)
	    break;
	mountPoint = Get_Next_In_Mount_Point_List(mountPoint);
    }

    Mutex_Unlock(&s_vfsLock);

    return mountPoint;
}

/*
 * Common implementation function for Open() and Open_Directory().
 */
static int Do_Open(
    const char *path, int mode, struct File **pFile,
    int (*openFunc)(struct Mount_Point *mountPoint, const char *path, int mode, struct File **pFile))
{
    char prefix[MAX_PREFIX_LEN + 1];
    const char *suffix;
    struct Mount_Point *mountPoint;
    int rc;

    if (!Unpack_Path(path, prefix, &suffix))
	return ENOTFOUND;

    /* Get mount point for path */
    mountPoint = Lookup_Mount_Point(prefix);
    if (mountPoint == 0)
	return ENOTFOUND;

    /* Call into actual Open() or Open_Directory() function. */
    rc = openFunc(mountPoint, suffix, mode, pFile);
    if (rc == 0) {
	/* File opened successfully! */
	(*pFile)->mode = mode;
	(*pFile)->mountPoint = mountPoint;
    }
    return rc;
}

/*
 * Adapter for Open().
 */
static int Do_Open_File(struct Mount_Point *mountPoint, const char *path, int mode, struct File **pFile)
{
    KASSERT(mountPoint->ops->Open != 0); /* All filesystems must implement Open(). */
    return mountPoint->ops->Open(mountPoint, path, mode, pFile);
}

/*
 * Adapter for Open_Directory().
 */
static int Do_Open_Directory(struct Mount_Point *mountPoint, const char *path, int mode, struct File **pDir)
{
    KASSERT(mountPoint->ops->Open_Directory != 0); /* All filesystems must implement Open_Directory(). */
    return mountPoint->ops->Open_Directory(mountPoint, path, pDir);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Register a filesystem.
 * This should only be called from Main(), before
 * any processes are started.
 * Params:
 *   fsName - name of the filesystem type, e.g. "pfat", "gosfs"
 *   fsOps - the Filesystem_Ops for the filesystem
 * Returns true if successful, false if not.
 */
bool Register_Filesystem(const char *fsName, struct Filesystem_Ops *fsOps)
{
    struct Filesystem *fs;

    KASSERT(fsName != 0);
    KASSERT(fsOps != 0);
    KASSERT(fsOps->Mount != 0);

    Debug("Registering %s filesystem type\n", fsName);

    /* Allocate Filesystem struct */
    fs = (struct Filesystem*) Malloc(sizeof(*fs));
    if (fs == 0)
	return false;

    /* Copy filesystem name and vtable. */
    fs->ops = fsOps;
    strncpy(fs->fsName, fsName, VFS_MAX_FS_NAME_LEN);
    fs->fsName[VFS_MAX_FS_NAME_LEN] = '\0';

    /* Add the filesystem to the list */
    Mutex_Lock(&s_vfsLock);
    Add_To_Back_Of_Filesystem_List(&s_filesystemList, fs);
    Mutex_Unlock(&s_vfsLock);

    return true;
}

/*
 * Format a block device using given filesystem type.
 * Params:
 *   devname - name of block device to format
 *   fstype - fstype, e.g. "pfat", "gosfs"
 * Returns: 0 if successful, error code (< 0) if not
 */
int Format(const char *devname, const char *fstype)
{
    struct Filesystem *fs;
    struct Block_Device *dev = 0;
    int rc;

    /* Find the named filesystem type */
    fs = Lookup_Filesystem(fstype);
    if (fs == 0)
	return ENOFILESYS;
    Debug("Found %s filesystem type\n", fstype);

    /* The Format() operation is optional. */
    if (fs->ops->Format == 0)
	return EUNSUPPORTED;

    /* Attempt to open the block device */
    if ((rc = Open_Block_Device(devname, &dev)) < 0)
	return rc;
    Debug("Opened device %s\n", dev->name);

    /* Dispatch to fs Format() function. */
    rc = fs->ops->Format(dev);

    Close_Block_Device(dev);

    return rc;
}

/*
 * Mount a filesystem on a block device.
 * Params:
 *   devname - block device name containing filesystem image
 *   pathPrefix - where to mount the filesystem in the overall namespace
 *   fstype - filesystem type, e.g. "pfat", "gosfs"
 * Returns: 0 if successful, error code (< 0) if not
 */
int Mount(const char *devname, const char *pathPrefix, const char *fstype)
{
    struct Filesystem *fs;
    struct Block_Device *dev = 0;
    struct Mount_Point *mountPoint = 0;
    int rc;

    /* Skip leading slash character(s) */
    while (*pathPrefix == '/')
	++pathPrefix;

    if (strlen(pathPrefix) > MAX_PREFIX_LEN)
	return ENAMETOOLONG;

    /* Find the named filesystem type */
    fs = Lookup_Filesystem(fstype);
    if (fs == 0)
	return ENOFILESYS;
    KASSERT(fs->ops->Mount != 0); /* All filesystems must implement Mount(). */

    /* Attempt to open the block device */
    if ((rc = Open_Block_Device(devname, &dev)) < 0)
	return rc;

    /* Create Mount_Point structure. */
    mountPoint = (struct Mount_Point*) Malloc(sizeof(*mountPoint));
    if (mountPoint == 0)
	goto memfail;
    memset(mountPoint, '\0', sizeof(*mountPoint));
    mountPoint->dev = dev;
    mountPoint->pathPrefix = strdup(pathPrefix);
    if (mountPoint->pathPrefix == 0)
	goto memfail;

    Debug("Mounting %s on %s using %s fs\n", devname, pathPrefix, fstype);

    /* Call the filesystem mount function. */
    if ((rc = fs->ops->Mount(mountPoint)) < 0)
	goto fail;

    Debug("Mount succeeded!\n");

    /*
     * Add filesystem to mount point list.
     * It is now ready to receive requests.
     * FIXME: should ensure that there aren't any filesystems
     * mounted on the same filesystem root.
     */
    Mutex_Lock(&s_vfsLock);
    Add_To_Back_Of_Mount_Point_List(&s_mountPointList, mountPoint);
    Mutex_Unlock(&s_vfsLock);

    return 0;

memfail:
    rc = ENOMEM;
fail:
    if (mountPoint != 0) {
	if (mountPoint->pathPrefix != 0)
	    Free(mountPoint->pathPrefix);
	Free(mountPoint);
    }
    if (dev != 0)
	Close_Block_Device(dev);
    return rc;
}

/*
 * Open a file.
 * Params:
 *   path - full path of the file
 *   mode - open flags: combination of O_CREATE, O_READ, O_WRITE, O_EXCL
 *   pFile - where to store pointer to File object if successful
 * Returns: 0 if successful, error code (< 0) if not
 */
int Open(const char *path, int mode, struct File **pFile)
{
    int rc = Do_Open(path, mode, pFile, &Do_Open_File);
    /*if (rc != 0) { Print("File open failed with code %d\n", rc); }*/
    return rc;
}

/*
 * Close a file or directory.  This will destroy the file object,
 * so it is important not to use the file again after this function
 * is called.
 * Params:
 *   file - the File to close
 * Returns: 0 if successful, error code (< 0) if not
 */
int Close(struct File *file)
{
    int rc;

    KASSERT(file->ops->Close != 0); /* All filesystems must implement Close(). */

    rc = file->ops->Close(file);
    if (rc == 0)
	Free(file);
    return rc;
}

/*
 * Get metadata for file specified by given path.
 * Params:
 *   path - path of file
 *   stat - pointer to VFS_File_Stat
 * Return: 0 if successful, error code (< 0) if not
 */
int Stat(const char *path, struct VFS_File_Stat *stat)
{
    char prefix[MAX_PREFIX_LEN + 1];
    const char *suffix;
    struct Mount_Point *mountPoint;

    if (!Unpack_Path(path, prefix, &suffix))
	return ENOTFOUND;

    /* Get mount point for path */
    Debug("Stat: lookup mount point for %s\n", prefix);
    mountPoint = Lookup_Mount_Point(prefix);
    if (mountPoint == 0)
	return ENOTFOUND;

    Debug("Stat: found mount point, dispatching to filesystem\n");
    if (mountPoint->ops->Stat == 0)
	return EUNSUPPORTED;
    else
	return mountPoint->ops->Stat(mountPoint, suffix, stat);
}

/*
 * Sync all mounted filesystems.
 * Returns: 0 if successful, error code (< 0) if not
 */
int Sync(void)
{
    int rc = 0;
    struct Mount_Point *mountPoint;

    Mutex_Lock(&s_vfsLock);
    for (mountPoint = Get_Front_Of_Mount_Point_List(&s_mountPointList);
	 mountPoint != 0;
	 mountPoint = Get_Next_In_Mount_Point_List(mountPoint)) {
	KASSERT(mountPoint->ops->Sync != 0);/* All filesystems must implement Sync */
	rc = mountPoint->ops->Sync(mountPoint);
	if (rc != 0)
	    break;
    }
    Mutex_Unlock(&s_vfsLock);

    return rc;
}

/*
 * Allocate a new File object.
 * Params:
 *   ops - the File_Ops for the file
 *   filePos - initial value for filePos
 *   endPos - initial value for endPos
 *   fsData - private data for use by filesystem implementation
 *   mode - file mode
 *   mountPoint - Mount_Point object of filesystem instance file belongs to
 *
 * Returns: new File object, or null if out of memory
 */
struct File *Allocate_File(struct File_Ops *ops, int filePos, int endPos, void *fsData,
    int mode, struct Mount_Point *mountPoint)
{
    struct File *file;

    file = (struct File *) Malloc(sizeof(struct File));
    if (file != 0) {
	file->ops = ops;
	file->filePos = filePos;
	file->endPos = endPos;
	file->fsData = fsData;
	file->mode = mode;
	file->mountPoint = mountPoint;
    }
    return file;
}

/*
 * Get metadata for given file.
 * Params:
 *   file - File object
 *   stat - pointer to VFS_File_Stat
 * Returns: 0 if successful, error code (< 0) if not
 */
int FStat(struct File *file, struct VFS_File_Stat *stat)
{
    if (file->ops->FStat == 0)
	return EUNSUPPORTED;
    else
	return file->ops->FStat(file, stat);
}

/*
 * Read bytes from the current position in a file.
 * Params:
 *   file - the File object
 *   buf - kernel buffer where data read from file should be stored
 *   len - number of bytes to read
 * Returns: number of bytes read, 0 if end-of-file is reached,
 *   or error code (< 0) if read fails
 */
int Read(struct File *file, void *buf, ulong_t len)
{
    if (file->ops->Read == 0)
	return EUNSUPPORTED;
    else
	return file->ops->Read(file, buf, len);
}

/*
 * Write bytes to the current position of a file.
 * Params:
 *   file - the File object
 *   buf - kernel buffer containing data to be written
 *   len - number of bytes to write
 * Returns: number of bytes written, or error code (< 0) if read fails
 */
int Write(struct File *file, void *buf, ulong_t len)
{
    if (file->ops->Write == 0)
	return EUNSUPPORTED;
    else
	return file->ops->Write(file, buf, len);
}

/*
 * Change current postion in file
 * Params:
 *   file - the File object
 *   len - new position
 * Returns: 0 if successful,
 *   or error code (< 0) if it fails
 */
int Seek(struct File *file, ulong_t len)
{
    if (file->ops->Seek == 0)
	return EUNSUPPORTED;
    else
	return file->ops->Seek(file, len);
}

/*
 * Completely read named file into a buffer.
 * Params:
 *   path - full path of file
 *   pBuffer - reference to variable where pointer to allocated buffer
 *     should be stored
 *   pLen - reference to variable where length of file should
 *     be stored
 * Returns: 0 if successful, error code (< 0) if not
 */
int Read_Fully(const char *path, void **pBuffer, ulong_t *pLen)
{
    struct File *file = 0;
    struct VFS_File_Stat stat;
    int rc;
    char *buf = 0;
    int numBytesRead;

    if ((rc = Stat(path, &stat)) < 0 || (rc = Open(path, O_READ, &file)) < 0)
	goto fail;
    if (stat.size < 0) {
	rc = ENOTFOUND;
	goto fail;
    }

    buf = (char*) Malloc(stat.size);
    if (buf == 0)
	goto memfail;

    /* Read until buffer is full */
    numBytesRead = 0;
    while (numBytesRead < stat.size) {
	rc = Read(file, buf + numBytesRead, stat.size - numBytesRead);
	if (rc < 0)
	    goto fail;
	numBytesRead += rc;
    }

    /* Success! */
    Close(file);
    *pBuffer = (void*) buf;
    *pLen = stat.size;
    return 0;

memfail:
    rc = ENOMEM;
fail:
    if (file != 0)
	Close(file);
    if (buf != 0)
	Free(buf);
    return rc;
}

/*
 * Create a directory.
 * Params:
 *   path - full path of directory to create
 * Returns: 0 if successful, error code (< 0) if not
 */
int Create_Directory(const char *path)
{
    char prefix[MAX_PREFIX_LEN + 1];
    const char *suffix;
    struct Mount_Point *mountPoint;

    /* Split path into prefix and suffix */
    if (!Unpack_Path(path, prefix, &suffix))
	return ENOTFOUND;

    /* Get mount point for path */
    mountPoint = Lookup_Mount_Point(prefix);
    if (mountPoint == 0)
	return ENOTFOUND;

    if (mountPoint->ops->Create_Directory == 0)
	return EUNSUPPORTED;
    else
	return mountPoint->ops->Create_Directory(mountPoint, suffix);
}

/*
 * Delete a file or directory
 * Params:
 *   path - full path of file/directory to delete
 * Returns: 0 if successful, error code (< 0) if not
 */
int Delete(const char *path)
{
    char prefix[MAX_PREFIX_LEN + 1];
    const char *suffix;
    struct Mount_Point *mountPoint;

    /* Split path into prefix and suffix */
    if (!Unpack_Path(path, prefix, &suffix))
	return ENOTFOUND;

    /* Get mount point for path */
    mountPoint = Lookup_Mount_Point(prefix);
    if (mountPoint == 0)
	return ENOTFOUND;

    if (mountPoint->ops->Delete == 0)
	return EUNSUPPORTED;
    else
	return mountPoint->ops->Delete(mountPoint, suffix);
}

/*
 * Open a directory.
 * Params:
 *   path - full path of directory
 *   pDir - where File object of opened directory should be stored
 * Returns: 0 if successful, error code (< 0) if not
 */
int Open_Directory(const char *path, struct File **pDir)
{
    return Do_Open(path, 0, pDir, &Do_Open_Directory);
}

/*
 * Read next directory entry.
 * Params:
 *   file - the File object representing the opened directory
 *   entry - pointer to VFS_Dir_Entry object
 * Returns: 0 if successful, error code (< 0) if not
 */
int Read_Entry(struct File *file, struct VFS_Dir_Entry *entry)
{
    if (file->ops->Read_Entry == 0)
	return EUNSUPPORTED;
    else
	return file->ops->Read_Entry(file, entry);
}

/*
 * Register a paging device.
 */
void Register_Paging_Device(struct Paging_Device *pagingDevice)
{
    KASSERT(s_pagingDevice == 0);
    KASSERT(pagingDevice != 0);
    Print("Registering paging device: %s on %s\n", pagingDevice->fileName, pagingDevice->dev->name);
    s_pagingDevice = pagingDevice;
}

/*
 * Get the paging device.
 * Returns null if no paging device has been registered.
 */
struct Paging_Device *Get_Paging_Device(void)
{
    return s_pagingDevice;
}

