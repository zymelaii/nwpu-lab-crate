/*
 * My feeble attempt at a floppy driver
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.22 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/screen.h>
#include <geekos/string.h>
#include <geekos/mem.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/irq.h>
#include <geekos/dma.h>
#include <geekos/io.h>
#include <geekos/timer.h>
#include <geekos/kthread.h>
#include <geekos/blockdev.h>
#include <geekos/floppy.h>

/*
 * NOTES:
 * The primary hardware target for this driver is Bochs 2.0.
 * Hopefully it will work on real hardware as well.
 */

/*
 * Information sources
 * - Jeff's original GeekOS floppy driver
 * - http://debs.future.easyspace.com/Programming/Hardware/FDC/floppy.html
 * - Intel 82077AA data sheet:
 *   http://www.nondot.org/sabre/os/files/Disk/82077AA_FloppyControllerDatasheet.pdf
 * - Frank van Gilluwe, _The Undocumented PC_, ISBN 0-201-47950-8, Chapter 10
 */

/*
 * TODO:
 * - Resetting the controller probably shouldn't require calibration,
 *   since there is no guarantee a disk will be in the drive.
 * - Need to really support multiple drives.
 */

/*
 * History:
 * 23-Oct-2003: Works under Bochs 2.0 for read transfers.
 * 12-Nov-2003: Modified to use block device API.
 */

/* ----------------------------------------------------------------------
 * Definitions
 * ---------------------------------------------------------------------- */

/*
 * Floppy controller IRQ
 */
#define FDC_IRQ				6

/*
 * Floppy DMA channel
 */
#define FDC_DMA				2

/*
 * I/O ports
 */
#define FDC_BASE			0x3F0
#define FDC_DOR_REG			0x3F2	 /*!< Digital output register (write) */
#define FDC_STATUS_REG			0x3F4	 /*!< Main status regiser (read) */
#define FDC_DATA_RATE_SELECT_REG	0x3F4	 /*!< Data rate select register (write) */
#define FDC_DATA_REG			0x3F5	 /*!< Data register (read, write) */

/*
 * Status register
 */
#define FDC_STATUS_MRQ			(1 << 7)
#define FDC_STATUS_DIO			(1 << 6)
#define FDC_STATUS_NDMA			(1 << 5)
#define FDC_STATUS_BUSY			(1 << 4)
#define FDC_STATUS_ACTIVE(drive)	(1 << (drive))
#define FDC_STATUS_READY_MASK		(FDC_STATUS_MRQ | FDC_STATUS_DIO)
#define FDC_STATUS_READY_WRITE		FDC_STATUS_MRQ
#define FDC_STATUS_READY_READ		(FDC_STATUS_MRQ | FDC_STATUS_DIO)

/*
 * DOR (Digital Output Register) bits
 */
#define FDC_DOR_MOTOR(drive)		(1 << ((drive) + 4))
#define FDC_DOR_DMA_ENABLE		(1 << 3)
#define FDC_DOR_RESET_DISABLE		(1 << 2)
#define FDC_DOR_DRIVE_SELECT(drive)	((drive) & 0x3)

/*
 * Commands
 */
#define FDC_COMMAND_CALIBRATE		0x07
#define FDC_COMMAND_SENSE_INT_STATUS	0x08
#define FDC_COMMAND_SEEK		0x0F
#define FDC_COMMAND_WRITE_SECTOR	0x05
#define FDC_COMMAND_READ_SECTOR		0x06

/*
 * Command bits
 */
#define FDC_MULTI_TRACK			0x80
#define FDC_MFM				0x40
#define FDC_SKIP_DELETED		0x20

/*
 * Status registers (from result phase)
 */
#define FDC_ST0_SEEK_END		(1 << 5)
#define FDC_ST0_IS_SUCCESS(code)	((((code) >> 6) & 0x3) == 0)

/*
 * CMOS configuration
 * See: http://www.osdever.net/tutorials/detecting_floppy_drives.php
 */
#define CMOS_OUT			0x70
#define CMOS_IN				0x71
#define CMOS_FLOPPY_INDEX		0x10

enum { FLOPPY_READ, FLOPPY_WRITE };

/*#define FLOPPY_DEBUG */
#ifdef FLOPPY_DEBUG
#  define Debug(args...) Print(args)
#else
#  define Debug(args...)
#endif

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

/*
 * Floppy drive parameters.
 */
struct Floppy_Parameters {
    int cylinders;
    int heads;
    int sectors;
    int sectorSizeCode;
    int gapLengthCode;
};
#define INVALID_FLOPPY_TYPE { -1, -1, -1, -1, -1 }

/*
 * Parameters of known floppy types.
 * These are indexed by the floppy type reported in the CMOS.
 */
static struct Floppy_Parameters s_floppyParamsTable[] = {
    INVALID_FLOPPY_TYPE,	 /* not installed */
    INVALID_FLOPPY_TYPE,	 /* TODO: 360K 5.25" */
    INVALID_FLOPPY_TYPE,	 /* TODO: 1.2M 5.25" */
    INVALID_FLOPPY_TYPE,	 /* TODO: 720K 3.5" */
    {80, 2, 18, 0x02, 0x1C},	 /* 1.44M 3.5" */
    INVALID_FLOPPY_TYPE,	 /* TODO: 2.88M 3.5" */
};
#define NUM_FLOPPY_TYPES (sizeof(s_floppyParamsTable) / sizeof(struct Floppy_Parameters))
#define IS_VALID_FLOPPY_TYPE(type) \
    ((type) < NUM_FLOPPY_TYPES && s_floppyParamsTable[(type)].cylinders > 0)

/*
 * Parameters and state information about a floppy drive.
 */
struct Floppy_Drive {
    struct Floppy_Parameters *params;
};

/*
 * Parameters and state information about installed drives.
 */
struct Floppy_Drive s_driveTable[2];

/*
 * Thread queue where a thread can wait to be notified that
 * a floppy interrupt has occurred.
 */
static struct Thread_Queue s_floppyInterruptWaitQueue;

/*
 * Page of memory used for floppy DMA.
 */
static uchar_t *s_transferBuf;

/*
 * Queue of floppy block I/O requests.
 */
static struct Block_Request_List s_floppyRequestQueue;

/*
 * Thread queue where request processing thread sleeps waiting for
 * a request to arrive.
 */
static struct Thread_Queue s_floppyWaitQueue;

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

/*
 * Implementation of Open for floppy driver.
 */
static int Floppy_Open(struct Block_Device *dev)
{
    KASSERT(!dev->inUse);
    return 0;
}

/*
 * Implementation of Close for floppy driver.
 */
static int Floppy_Close(struct Block_Device *dev)
{
    KASSERT(dev->inUse);
    return 0;
}

/*
 * Implementation of Get_Num_Blocks for floppy driver.
 */
static int Floppy_Get_Num_Blocks(struct Block_Device *dev)
{
    struct Floppy_Drive *drive;
    struct Floppy_Parameters *params = drive->params;

    KASSERT(dev->unit >= 0 && dev->unit <= 1);
    drive = &s_driveTable[dev->unit];

    params = drive->params;
    KASSERT(params != 0);

    return params->cylinders * params->heads * params->sectors;
}

/*
 * Block_Device_Ops for floppy driver.
 */
static struct Block_Device_Ops s_floppyDeviceOps = {
    Floppy_Open,
    Floppy_Close,
    Floppy_Get_Num_Blocks,
};

/*
 * Interrupt handler.
 * The floppy controller generally issues an interrupt
 * to notify the driver of the completion of a command.
 */
/*
 * Wake up any threads in the floppy wait queue.
 */
static void Floppy_Interrupt_Handler(struct Interrupt_State* state)
{
    Begin_IRQ(state);
    Wake_Up(&s_floppyInterruptWaitQueue);
    End_IRQ(state);
}

/*
 * Initialize drive parameters based on the floppy type returned
 * by the CMOS.
 */
static void Setup_Drive_Parameters(int drive, int type)
{
    if (IS_VALID_FLOPPY_TYPE(type)) {
	struct Floppy_Parameters* params = &s_floppyParamsTable[type];
	char devname[BLOCKDEV_MAX_NAME_LEN+1];
	int rc;

	snprintf(devname, sizeof(devname), "fd%d", drive);
	Print("    %s: cyl=%d, heads=%d, sectors=%d\n", devname,
		 params->cylinders, params->heads, params->sectors);
	s_driveTable[drive].params = params;

	/* Register the block device. */
	rc = Register_Block_Device(devname, &s_floppyDeviceOps, drive, 0,
	    &s_floppyWaitQueue, &s_floppyRequestQueue);
	if (rc != 0)
	    Print("  Error: could not create block device for %s\n", devname);
    }
}

/*
 * Convert LBA address to CHS.
 */
static void LBA_To_CHS(struct Floppy_Drive* drive, int lba, int *cylinder, int *head, int *sector)
{
    struct Floppy_Parameters* params = drive->params;

    KASSERT(params != 0);

    *cylinder = lba / (params->heads * params->sectors);
    *head = (lba / params->sectors) % params->heads;
    *sector = (lba % params->sectors) + 1;

    KASSERT(*cylinder >= 0 && *cylinder < params->cylinders);
    KASSERT(*head >= 0 && *head < params->heads);
    KASSERT(*sector > 0 && *sector <= params->sectors);
}

/*
 * Wait for the MRQ bit to be set in the main status register.
 * This indicates that the controller is ready to accept data
 * or send data via the data register.
 */
static void Wait_For_MRQ(uchar_t readyValue)
{
    KASSERT(readyValue == FDC_STATUS_READY_READ || readyValue == FDC_STATUS_READY_WRITE);

    /* Wait for MRQ bit to be set in main status register */
    while ((In_Byte(FDC_STATUS_REG) & FDC_STATUS_READY_MASK) != readyValue)
	;

    /*Debug("Ready to accept command!\n"); */
}

/*
 * Get a byte from the data register.
 */
static uchar_t Floppy_In(void)
{
    Wait_For_MRQ(FDC_STATUS_READY_READ);
    return In_Byte(FDC_DATA_REG);
}

/*
 * Write a byte to the data register.
 */
static void Floppy_Out(uchar_t val)
{
    Wait_For_MRQ(FDC_STATUS_READY_WRITE);
    Out_Byte(FDC_DATA_REG, val);
}

/*
 * Wait for the controller to issue an interrupt.
 * Must be called with interrupts disabled.
 */
static void Wait_For_Interrupt(void)
{
    KASSERT(!Interrupts_Enabled());

    /* Wait for interrupt */
    Wait(&s_floppyInterruptWaitQueue);
}

static void Sense_Interrupt_Status(uchar_t* st0, uchar_t *pcn)
{
    Floppy_Out(FDC_COMMAND_SENSE_INT_STATUS);
    *st0 = Floppy_In();
    *pcn = Floppy_In();
}

/*
 * Calibrate the given drive.
 */
static bool Calibrate(int drive)
{
    int numAttempts = 4;
    bool success = false;
    uchar_t st0, pcn;

    KASSERT(!Interrupts_Enabled());

    while (numAttempts-- > 0) {
	/* Issue the calibrate command */
	Floppy_Out(FDC_COMMAND_CALIBRATE);
	Floppy_Out((uchar_t) drive);
	Wait_For_Interrupt();

	/* Check interrupt status, to see if calibrate succeeded */
	Sense_Interrupt_Status(&st0, &pcn);
	Debug("Calibrate: st0=%02x, pcn=%02x\n", st0, pcn);
	if (st0 & FDC_ST0_SEEK_END) {
	    success = true;
	    break;
	}
    }

    Debug("Drive %d calibration %s\n", drive, success?"succeeded":"failed");
    return success;
}

static void Start_Motor(int drive)
{
    Out_Byte(FDC_DOR_REG,
	FDC_DOR_MOTOR(drive) | FDC_DOR_DMA_ENABLE | FDC_DOR_RESET_DISABLE | FDC_DOR_DRIVE_SELECT(0));
}

static void Stop_Motor(int drive)
{
    Out_Byte(FDC_DOR_REG,
	FDC_DOR_DMA_ENABLE | FDC_DOR_RESET_DISABLE | FDC_DOR_DRIVE_SELECT(0));
}

/*
 * Reset and calibrate the controller.
 * Return true is successful, false otherwise.
 */
static bool Reset_Controller(void)
{
    /* Reset */
    Out_Byte(FDC_DOR_REG, 0);
    /*Micro_Delay(1000); */

    /*
     * Enable fd0
     * TODO: we might want to support drives other than 0 eventually
     */
    Start_Motor(0);

    return Calibrate(0);
}

static bool Floppy_Seek(int drive, int cylinder, int head)
{
    uchar_t st0, pcn;
    int numAttempts = 4;
    bool success = false;

    Debug("Floppy_Seek(%d,%d,%d)\n", drive, cylinder, head);

    while (numAttempts-- > 0) {
	Start_Motor(drive);
	/*Micro_Delay(1000); */

	Disable_Interrupts();

	Floppy_Out(FDC_COMMAND_SEEK);
	Floppy_Out((head << 2) | (drive & 3));
	Floppy_Out(cylinder & 0xFF);

	Debug("Seek: waiting for interrupt\n");
	Wait_For_Interrupt();
	Debug("Seek: got interrupt\n");

	Enable_Interrupts();

	Stop_Motor(drive);

	Sense_Interrupt_Status(&st0, &pcn);
	if (st0 & FDC_ST0_SEEK_END) {
	    /* Make sure we arrived at the desired cylinder */
	    if (pcn != cylinder) {
		Debug("Seek arrived at wrong cylinder\n");
	    } else {
		Debug("Seek complete!\n");
		success = true;
		break;
	    }
	}
    }

    return success;
}

static int Floppy_Transfer(int direction, int driveNum, int blockNum, char *buf)
{
    struct Floppy_Drive *drive = &s_driveTable[driveNum];
    struct Floppy_Parameters *params = drive->params;
    int cylinder, head, sector;
    enum DMA_Direction dmaDirection =
	direction == FLOPPY_READ ? DMA_READ : DMA_WRITE;
    uchar_t command;
    uchar_t st0, st1, st2;
    int result = -1;

    KASSERT(driveNum == 0);  /* FIXME */
    KASSERT(direction == FLOPPY_READ || direction == FLOPPY_WRITE);
    KASSERT(params != 0);

    LBA_To_CHS(&s_driveTable[driveNum], blockNum, &cylinder, &head, &sector);

    if (!Floppy_Seek(driveNum, cylinder, head))
	return -1;

    Disable_Interrupts();

    /* Set up DMA for transfer */
    Setup_DMA(dmaDirection, FDC_DMA, s_transferBuf, SECTOR_SIZE);

    /* Turn the floppy motor on */
    Start_Motor(driveNum);

    /*
     * According to The Undocumented PC, we should wait 8 millis
     * before attempting a read or write.
     */
    Micro_Delay(8000);

    if (direction == FLOPPY_READ)
	command = FDC_COMMAND_READ_SECTOR | FDC_MFM | FDC_SKIP_DELETED;
    else
	command = FDC_COMMAND_WRITE_SECTOR | FDC_MFM;
 
    /* Issue the command */
    Floppy_Out(command);
    Floppy_Out((head << 2) | (driveNum & 3));
    Floppy_Out(cylinder);
    Floppy_Out(head);
    Floppy_Out(sector);
    Floppy_Out(params->sectorSizeCode);
    Floppy_Out(params->sectors);
    Floppy_Out(params->gapLengthCode);
    Floppy_Out(0xFF);  /* DTL */

    /* Controller will issue an interrupt when the command is complete */
    Wait_For_Interrupt();
    Debug("Floppy_Transfer: received interrupt!\n");

    /* Read results */
    st0 = Floppy_In();
    st1 = Floppy_In();
    st2 = Floppy_In();
    Floppy_In();  /* cylinder */
    Floppy_In();  /* head */
    Floppy_In();  /* sector number */
    Floppy_In();  /* sector size */

    Stop_Motor(driveNum);

    if (FDC_ST0_IS_SUCCESS(st0)) {
	Debug("Floppy_Transfer: successful transfer!\n");
	result = 0;
    }

    Enable_Interrupts();

    /*STOP(); */
    return result;
}

static int Floppy_Read(int driveNum, int blockNum, char *buffer)
{
    int rc;

    Debug("Floppy_Read(%d,%d,%x)\n", driveNum, blockNum, buffer);

#ifndef NDEBUG
    memset(buffer, (char) 0xcd, SECTOR_SIZE);
    memset(s_transferBuf, (char) 0xcd, SECTOR_SIZE);
#endif

    rc = Floppy_Transfer(FLOPPY_READ, driveNum, blockNum, buffer);

    if (rc == 0) {
	/*
	 * Successful transfer!
	 * Copy data from transfer buffer into caller's buffer.
	 */
	memcpy(buffer, s_transferBuf, SECTOR_SIZE);
    }

    return rc;
}

static int Floppy_Write(int driveNum, int blockNum, char *buffer)
{
    Debug("Floppy_Write(%d,%d,%x)\n", driveNum, blockNum, buffer);

    memcpy(s_transferBuf, buffer, SECTOR_SIZE);
    return Floppy_Transfer(FLOPPY_WRITE, driveNum, blockNum, buffer);
}

/*
 * This is the thread which processes floppy I/O requests.
 */
static void Floppy_Request_Thread(ulong_t arg)
{
    int rc;

    Debug("FRQ: Floppy request thread starting...\n");

    for (;;) {
	struct Block_Request *request;

	/* Wait for an I/O request to arrive */
	Debug("FRQ: Request thread waiting for a request\n");
	request = Dequeue_Request(&s_floppyRequestQueue, &s_floppyWaitQueue);
	Debug("FRQ: Got a floppy request [@%x]\n", request);
	KASSERT(request->type == BLOCK_READ || request->type == BLOCK_WRITE);

	/* Perform the I/O. */
	if (request->type == BLOCK_READ)
	    rc = Floppy_Read(request->dev->unit, request->blockNum, request->buf);
	else
	    rc = Floppy_Write(request->dev->unit, request->blockNum, request->buf);

	/* Notify the requesting thread of the outcome of the I/O. */
	Debug("FRQ: Notifying requesting thread...\n");
	Notify_Request_Completion(request, rc == 0 ? COMPLETED : ERROR, rc);
	Debug("FRQ: Completed floppy request\n");
    }
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize the floppy controller.
 */
void Init_Floppy(void)
{
    uchar_t floppyByte;
    bool ready = false;
    bool good;

    Print("Initializing floppy controller...\n");

    /* Allocate memory for DMA transfers */
    s_transferBuf = (uchar_t*) Alloc_Page();

    /* Use CMOS to get floppy configuration */
    Out_Byte(CMOS_OUT, CMOS_FLOPPY_INDEX);
    floppyByte = In_Byte(CMOS_IN);
    Setup_Drive_Parameters(0, (floppyByte >> 4) & 0xF);
    Setup_Drive_Parameters(1, floppyByte & 0xF);

    /* Install floppy interrupt handler */
    Install_IRQ(FDC_IRQ, &Floppy_Interrupt_Handler);
    Enable_IRQ(FDC_IRQ);

    /* Reset and calibrate the controller. */
    Disable_Interrupts();
    good = Reset_Controller();
    Enable_Interrupts();
    if (!good) {
	Print("  Failed to reset controller!\n");
	goto done;
    }

    /* Reserve DMA channel 2. */
    if (!Reserve_DMA(FDC_DMA)) {
	Print("  Failed to reserve DMA channel\n");
	goto done;
    }

    /*
     * Driver is now ready for requests.
     * Start the request processing thread.
     */
    ready = true;
    Start_Kernel_Thread(Floppy_Request_Thread, 0, PRIORITY_NORMAL, true);

done:
    if (!ready)
	Print("  Floppy controller initialization FAILED\n");
}

