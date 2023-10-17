/*
 * ATA (aka IDE) driver.
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * NOTES:
 * 12/22/03 - Converted to use new block device layer with queued requests
 *  1/20/04 - Changed probing of drives to work on Bochs 2.0 with 2 drives
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/errno.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/io.h>
#include <geekos/int.h>
#include <geekos/screen.h>
#include <geekos/timer.h>
#include <geekos/kthread.h>
#include <geekos/blockdev.h>
#include <geekos/ide.h>

/* Registers */
#define IDE_DATA_REGISTER		0x1f0
#define IDE_ERROR_REGISTER		0x1f1
#define IDE_FEATURE_REG			IDE_ERROR_REGISTER
#define IDE_SECTOR_COUNT_REGISTER	0x1f2
#define IDE_SECTOR_NUMBER_REGISTER	0x1f3
#define IDE_CYLINDER_LOW_REGISTER	0x1f4
#define IDE_CYLINDER_HIGH_REGISTER	0x1f5
#define IDE_DRIVE_HEAD_REGISTER		0x1f6
#define IDE_STATUS_REGISTER		0x1f7
#define IDE_COMMAND_REGISTER		0x1f7
#define IDE_DEVICE_CONTROL_REGISTER	0x3F6

/* Drives */
#define IDE_DRIVE_0			0xa0
#define IDE_DRIVE_1			0xb0

/* Commands */
#define IDE_COMMAND_IDENTIFY_DRIVE	0xEC
#define IDE_COMMAND_SEEK		0x70
#define IDE_COMMAND_READ_SECTORS	0x21
#define IDE_COMMAND_READ_BUFFER		0xE4
#define IDE_COMMAND_WRITE_SECTORS	0x30
#define IDE_COMMAND_WRITE_BUFFER	0xE8
#define IDE_COMMAND_DIAGNOSTIC		0x90
#define IDE_COMMAND_ATAPI_IDENT_DRIVE	0xA1

/* Results words from Identify Drive Request */
#define	IDE_INDENTIFY_NUM_CYLINDERS	0x01
#define	IDE_INDENTIFY_NUM_HEADS		0x03
#define	IDE_INDENTIFY_NUM_BYTES_TRACK	0x04
#define	IDE_INDENTIFY_NUM_BYTES_SECTOR	0x05
#define	IDE_INDENTIFY_NUM_SECTORS_TRACK	0x06

/* bits of Status Register */
#define IDE_STATUS_DRIVE_BUSY		0x80
#define IDE_STATUS_DRIVE_READY		0x40
#define IDE_STATUS_DRIVE_WRITE_FAULT	0x20
#define IDE_STATUS_DRIVE_SEEK_COMPLETE	0x10
#define IDE_STATUS_DRIVE_DATA_REQUEST	0x08
#define IDE_STATUS_DRIVE_CORRECTED_DATA	0x04
#define IDE_STATUS_DRIVE_INDEX		0x02
#define IDE_STATUS_DRIVE_ERROR		0x01

/* Bits of Device Control Register */
#define IDE_DCR_NOINTERRUPT		0x02
#define IDE_DCR_RESET			0x04

/* Return codes from various IDE_* functions */
#define	IDE_ERROR_NO_ERROR	0
#define	IDE_ERROR_BAD_DRIVE	-1
#define	IDE_ERROR_INVALID_BLOCK	-2
#define	IDE_ERROR_DRIVE_ERROR	-3

/* Control register bits */
#define IDE_CONTROL_REGISTER		0x3F6
#define IDE_CONTROL_SOFTWARE_RESET	0x04
#define IDE_CONTROL_INT_DISABLE		0x02

#define LOW_BYTE(x)	(x & 0xff)
#define HIGH_BYTE(x)	((x >> 8) & 0xff)

#define IDE_MAX_DRIVES			2

typedef struct {
    short num_Cylinders;
    short num_Heads;
    short num_SectorsPerTrack;
    short num_BytesPerSector;
} ideDisk;

int ideDebug = 0;
static int numDrives;
static ideDisk drives[IDE_MAX_DRIVES];

struct Thread_Queue s_ideWaitQueue;
struct Block_Request_List s_ideRequestQueue;

/*
 * return the number of logical blocks for a particular drive.
 *
 */
static int IDE_getNumBlocks(int driveNum)
{
    if (driveNum < 0 || driveNum > IDE_MAX_DRIVES) {
        return IDE_ERROR_BAD_DRIVE;
    }

    return (drives[driveNum].num_Heads * 
            drives[driveNum].num_SectorsPerTrack *
	    drives[driveNum].num_Cylinders);
}

/*
 * Read a block at the logical block number indicated.
 */
static int IDE_Read(int driveNum, int blockNum, char *buffer)
{
    int i;
    int head;
    int sector;
    int cylinder;
    short *bufferW;
    int reEnable = 0;

    if (driveNum < 0 || driveNum > (numDrives-1)) {
	if (ideDebug) Print("ide: invalid drive %d\n", driveNum);
        return IDE_ERROR_BAD_DRIVE;
    }

    if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
	if (ideDebug) Print("ide: invalid block %d\n", blockNum);
        return IDE_ERROR_INVALID_BLOCK;
    }

    if (Interrupts_Enabled()) {
	Disable_Interrupts();
	reEnable = 1;
    }

    /* now compute the head, cylinder, and sector */
    sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
    cylinder = blockNum / (drives[driveNum].num_Heads * 
     	drives[driveNum].num_SectorsPerTrack);
    head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
        drives[driveNum].num_Heads;

    if (ideDebug >= 2) {
	Print ("request to read block %d\n", blockNum);
	Print ("    head %d\n", head);
	Print ("    cylinder %d\n", cylinder);
	Print ("    sector %d\n", sector);
    }

    Out_Byte(IDE_SECTOR_COUNT_REGISTER, 1);
    Out_Byte(IDE_SECTOR_NUMBER_REGISTER, sector);
    Out_Byte(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
    Out_Byte(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
    if (driveNum == 0) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
    } else if (driveNum == 1) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
    }

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_READ_SECTORS);

    if (ideDebug > 2) Print("About to wait for Read \n");

    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    if (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
	Print("ERROR: Got Read %d\n", In_Byte(IDE_STATUS_REGISTER));
	return IDE_ERROR_DRIVE_ERROR;
    }

    if (ideDebug > 2) Print("got buffer \n");

    bufferW = (short *) buffer;
    for (i=0; i < 256; i++) {
        bufferW[i] = In_Word(IDE_DATA_REGISTER);
    }

    if (reEnable) Enable_Interrupts();

    return IDE_ERROR_NO_ERROR;
}

/*
 * Write a block at the logical block number indicated.
 */
static int IDE_Write(int driveNum, int blockNum, char *buffer)
{
    int i;
    int head;
    int sector;
    int cylinder;
    short *bufferW;
    int reEnable = 0;

    if (driveNum < 0 || driveNum > (numDrives-1)) {
        return IDE_ERROR_BAD_DRIVE;
    }

    if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
        return IDE_ERROR_INVALID_BLOCK;
    }

    if (Interrupts_Enabled()) {
	Disable_Interrupts();
	reEnable = 1;
    }

    /* now compute the head, cylinder, and sector */
    sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
    cylinder = blockNum / (drives[driveNum].num_Heads * 
     	drives[driveNum].num_SectorsPerTrack);
    head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
        drives[driveNum].num_Heads;

    if (ideDebug) {
	Print ("request to write block %d\n", blockNum);
	Print ("    head %d\n", head);
	Print ("    cylinder %d\n", cylinder);
	Print ("    sector %d\n", sector);
    }

    Out_Byte(IDE_SECTOR_COUNT_REGISTER, 1);
    Out_Byte(IDE_SECTOR_NUMBER_REGISTER, sector);
    Out_Byte(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
    Out_Byte(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
    if (driveNum == 0) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
    } else if (driveNum == 1) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
    }

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_WRITE_SECTORS);


    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    bufferW = (short *) buffer;
    for (i=0; i < 256; i++) {
        Out_Word(IDE_DATA_REGISTER, bufferW[i]);
    }

    if (ideDebug) Print("About to wait for Write \n");

    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    if (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
	Print("ERROR: Got Read %d\n", In_Byte(IDE_STATUS_REGISTER));
	return IDE_ERROR_DRIVE_ERROR;
    }

    if (reEnable) Enable_Interrupts();

    return IDE_ERROR_NO_ERROR;
}

static int IDE_Open(struct Block_Device *dev)
{
    KASSERT(!dev->inUse);
    return 0;
}

static int IDE_Close(struct Block_Device *dev)
{
    KASSERT(dev->inUse);
    return 0;
}

static int IDE_Get_Num_Blocks(struct Block_Device *dev)
{
    return IDE_getNumBlocks(dev->unit);
}

static struct Block_Device_Ops s_ideDeviceOps = {
    IDE_Open,
    IDE_Close,
    IDE_Get_Num_Blocks,
};

static void IDE_Request_Thread(ulong_t arg)
{
    for (;;) {
	struct Block_Request *request;
	int rc;

	/* Wait for a request to arrive */
	request = Dequeue_Request(&s_ideRequestQueue, &s_ideWaitQueue);

	/* Do the I/O */
	if (request->type == BLOCK_READ)
	    rc = IDE_Read(request->dev->unit, request->blockNum, request->buf);
	else
	    rc = IDE_Write(request->dev->unit, request->blockNum, request->buf);

	/* Notify requesting thread of final status */
	Notify_Request_Completion(request, rc == 0 ? COMPLETED : ERROR, rc);
    }
}

static int readDriveConfig(int drive)
{
    int i;
    int status;
    short info[256];
    char devname[BLOCKDEV_MAX_NAME_LEN];
    int rc;

    if (ideDebug > 1) Print("ide: about to read drive config for drive #%d\n", drive);

    Out_Byte(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_IDENTIFY_DRIVE);
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    status = In_Byte(IDE_STATUS_REGISTER);
    /*
     * simulate failure
     * status = 0x50;
     */
    if ((status & IDE_STATUS_DRIVE_DATA_REQUEST)) {
       /*Print("ide: probe found ATA drive\n");*/
         /* drive responded to ATA probe */
	for (i=0; i < 256; i++) {
	    info[i] = In_Word(IDE_DATA_REGISTER);
	}

	drives[drive].num_Cylinders = info[IDE_INDENTIFY_NUM_CYLINDERS];
	drives[drive].num_Heads = info[IDE_INDENTIFY_NUM_HEADS];
	drives[drive].num_SectorsPerTrack = info[IDE_INDENTIFY_NUM_SECTORS_TRACK];
	drives[drive].num_BytesPerSector = info[IDE_INDENTIFY_NUM_BYTES_SECTOR];
    } else {
       /* try for ATAPI */
       Out_Byte(IDE_FEATURE_REG, 0);		 /* disable dma & overlap */

       Out_Byte(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
       Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_ATAPI_IDENT_DRIVE);
       while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
       status = In_Byte(IDE_STATUS_REGISTER);
       /*Print("ide: found atapi drive\n");*/
       return -1;
    }

    Print("    ide%d: cyl=%d, heads=%d, sectors=%d\n", drive, drives[drive].num_Cylinders,
	drives[drive].num_Heads, drives[drive].num_SectorsPerTrack);

    /* Register the drive as a block device */
    snprintf(devname, sizeof(devname), "ide%d", drive);
    rc = Register_Block_Device(devname, &s_ideDeviceOps, drive, 0, &s_ideWaitQueue, &s_ideRequestQueue);
    if (rc != 0)
	Print("  Error: could not create block device for %s\n", devname);

    return 0;
}


void Init_IDE(void)
{
    int errorCode;

    Print("Initializing IDE controller...\n");

    /* Reset the controller and drives */
    Out_Byte(IDE_DEVICE_CONTROL_REGISTER, IDE_DCR_NOINTERRUPT | IDE_DCR_RESET);
    Micro_Delay(100);
    Out_Byte(IDE_DEVICE_CONTROL_REGISTER, IDE_DCR_NOINTERRUPT);

/*
 * FIXME: This code doesn't work on Bochs 2.0.
 *    while ((In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_READY) == 0)
 *	;
 */

    /* This code does work on Bochs 2.0. */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY)
	;

    if (ideDebug) Print("About to run drive Diagnosis\n");

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_DIAGNOSTIC);
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
    errorCode = In_Byte(IDE_ERROR_REGISTER);
    if (ideDebug > 1) Print("ide: ide error register = %x\n", errorCode);

    /* Probe and register drives */
    if (readDriveConfig(0) == 0)
	++numDrives;
    if (readDriveConfig(1) == 0)
	++numDrives;
    if (ideDebug) Print("Found %d IDE drives\n", numDrives);

    /* Start request thread */
    if (numDrives > 0)
	Start_Kernel_Thread(IDE_Request_Thread, 0, PRIORITY_NORMAL, true);
}
