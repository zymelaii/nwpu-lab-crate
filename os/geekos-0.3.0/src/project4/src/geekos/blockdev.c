/*
 * Block devices
 * Copyright (c) 2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.17 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/screen.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/kthread.h>
#include <geekos/synch.h>
#include <geekos/blockdev.h>

/*#define BLOCKDEV_DEBUG */
#ifdef BLOCKDEV_DEBUG
#  define Debug(args...) Print(args)
#else
#  define Debug(args...)
#endif

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * Lock protecting access/modification of block device list.
 */
static struct Mutex s_blockdevLock;

/*
 * List datatype for list of block devices.
 */
DEFINE_LIST(Block_Device_List, Block_Device);
IMPLEMENT_LIST(Block_Device_List, Block_Device);

/*
 * The list in which all block devices in the system
 * are registered.
 */
static struct Block_Device_List s_deviceList;

/*
 * Perform a block IO request.
 * Returns 0 if successful, error code on failure.
 */
static int Do_Request(struct Block_Device *dev, enum Request_Type type, int blockNum, void *buf)
{
    struct Block_Request *request;
    int rc;

    request = Create_Request(dev, type, blockNum, buf);
    if (request == 0)
	return ENOMEM;
    Post_Request_And_Wait(request);
    rc = request->errorCode;
    Free(request);
    return rc;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Register a block device.
 * This should be called by device drivers in their Init
 * functions to register all detected devices.
 * Returns 0 if successful, error code otherwise.
 */
int Register_Block_Device(const char *name, struct Block_Device_Ops *ops,
    int unit, void *driverData, struct Thread_Queue *waitQueue,
    struct Block_Request_List *requestQueue)
{
    struct Block_Device *dev;

    KASSERT(ops != 0);
    KASSERT(waitQueue != 0);
    KASSERT(requestQueue != 0);

    dev = (struct Block_Device*) Malloc(sizeof(*dev));
    if (dev == 0)
	return ENOMEM;

    strcpy(dev->name, name);
    dev->ops = ops;
    dev->unit = unit;
    dev->inUse = false;
    dev->driverData = driverData;
    dev->waitQueue = waitQueue;
    dev->requestQueue = requestQueue;

    Mutex_Lock(&s_blockdevLock);
    /* FIXME: handle name conflict with existing device */
    Debug("Registering block device %s\n", dev->name);
    Add_To_Back_Of_Block_Device_List(&s_deviceList, dev);
    Mutex_Unlock(&s_blockdevLock);

    return 0;
}

/*
 * Open a named block device.
 * Return 0 if successful, error code on error.
 */
int Open_Block_Device(const char *name, struct Block_Device **pDev)
{
    struct Block_Device *dev;
    int rc = 0;

    Mutex_Lock(&s_blockdevLock);

    dev = Get_Front_Of_Block_Device_List(&s_deviceList);
    while (dev != 0) {
	if (strcmp(dev->name, name) == 0)
	    break;
	dev = Get_Next_In_Block_Device_List(dev);
    }

    if (dev == 0)
	rc = ENODEV;
    else if (dev->inUse)
	rc = EBUSY;
    else {
	rc = dev->ops->Open(dev);
	if (rc == 0) {
	    *pDev = dev;
	    dev->inUse = true;
	}
    }

    Mutex_Unlock(&s_blockdevLock);

    return rc;
}

/*
 * Close given block device.
 * Return 0 if successful, error code on error.
 */
int Close_Block_Device(struct Block_Device *dev)
{
    int rc;

    Mutex_Lock(&s_blockdevLock);

    KASSERT(dev->inUse);
    rc = dev->ops->Close(dev);
    if (rc == 0)
	dev->inUse = false;

    Mutex_Unlock(&s_blockdevLock);

    return rc;
}

/*
 * Create a block device request to transfer a single block.
 */
struct Block_Request *Create_Request(struct Block_Device *dev, enum Request_Type type,
    int blockNum, void *buf)
{
    struct Block_Request *request = Malloc(sizeof(*request));
    if (request != 0) {
	request->dev = dev;
	request->type = type;
	request->blockNum = blockNum;
	request->buf = buf;
	request->state = PENDING;
	Clear_Thread_Queue(&request->waitQueue);
    }
    return request;
}

/*
 * Send a block IO request to a device and wait for it to be handled.
 * Returns when the driver completes the requests or signals
 * an error.
 */
void Post_Request_And_Wait(struct Block_Request *request)
{
    struct Block_Device *dev;

    KASSERT(request != 0);

    dev = request->dev;
    KASSERT(dev != 0);

    /* Send request to the driver */
    Debug("Posting block device request [@%x]...\n", request);
    Disable_Interrupts();
    Add_To_Back_Of_Block_Request_List(dev->requestQueue, request);
    Wake_Up(dev->waitQueue);
    Enable_Interrupts();

    /* Wait for request to be processed */
    Disable_Interrupts();
    while (request->state == PENDING) {
	Debug("Waiting, state=%d\n", request->state);
	Wait(&request->waitQueue);
    }
    Debug("Wait completed!\n");
    Enable_Interrupts();
}

/*
 * Wait for a block request to arrive.
 */
struct Block_Request *Dequeue_Request(struct Block_Request_List *requestQueue,
    struct Thread_Queue *waitQueue)
{
    struct Block_Request *request;

    Disable_Interrupts();
    while (Is_Block_Request_List_Empty(requestQueue))
	Wait(waitQueue);
    request = Get_Front_Of_Block_Request_List(requestQueue);
    Remove_From_Front_Of_Block_Request_List(requestQueue);
    Enable_Interrupts();

    return request;
}

/*
 * Signal the completion of a block request.
 */
void Notify_Request_Completion(struct Block_Request *request, enum Request_State state, int errorCode)
{
    Disable_Interrupts();
    request->state = state;
    request->errorCode = errorCode;
    Wake_Up(&request->waitQueue);
    Enable_Interrupts();
}

/*
 * Read a block from given device.
 * Return 0 if successful, error code on error.
 */
int Block_Read(struct Block_Device *dev, int blockNum, void *buf)
{
    return Do_Request(dev, BLOCK_READ, blockNum, buf);
}

/*
 * Write a block to given device.
 * Return 0 if successful, error code on error.
 */
int Block_Write(struct Block_Device *dev, int blockNum, void *buf)
{
    return Do_Request(dev, BLOCK_WRITE, blockNum, buf);
}

/*
 * Get number of blocks in given device.
 */
int Get_Num_Blocks(struct Block_Device *dev)
{
    return dev->ops->Get_Num_Blocks(dev);
}

