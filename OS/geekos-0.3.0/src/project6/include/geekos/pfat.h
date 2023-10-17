/*
 * Header file for the pseudo-fat filesystem.
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_PFAT_H
#define GEEKOS_PFAT_H

/*
 * WARNING: When changing this code, check out setup.asm and bootsect.asm,
 *  which use fields of this structure!
 */
typedef struct {
    int magic;			/* id to tell the type of filesystem */
    int fileAllocationOffset;	/* where is the file allocation table */
    int fileAllocationLength;	/* length of allocation table */
    int rootDirectoryOffset;	/* offset in sectors of root directory */
    int rootDirectoryCount;	/* number of items in the directory */
    short setupStart;		/* first sector of secondary loader */
    short setupSize;		/* size in sectors of secondary loader */
    short kernelStart;		/* first sector of kernel to run */
    short kernelSize;		/* size in sectors of kernel to run */
} bootSector;

typedef struct {
    char fileName[8+4];

    /* attribute bits */
    char readOnly:1;
    char hidden:1;
    char systemFile:1;
    char volumeLabel:1;
    char directory:1;

    short time;
    short date;
    int firstBlock;
    int fileSize;
} directoryEntry;

#define FAT_ENTRY_FREE		0
#define FAT_ENTRY_EOF		1

/* magic number to indicate its a PFAT disk */
#define PFAT_MAGIC		0x78320000

/* where in the boot sector is the pfat record */
#define PFAT_BOOT_RECORD_OFFSET 482

void Init_PFAT(void);

#endif  /* GEEKOS_PFAT_H */
