/*
 * Pipe pseudo-filesystem
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.12 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/kassert.h>
#include <geekos/errno.h>
#include <geekos/vfs.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/pipefs.h>

/* The amount of storage to allocate for a pipe. */
#define PIPE_BUF_SIZE 4096

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * Read data from a pipe.
 * Returns number of bytes read, or 0 if end-of-file
 * has been reached.
 */
static int Pipe_Read(struct File *file, void *buf, ulong_t numBytes)
{
    TODO("Read from pipe");
}

/*
 * Write data to pipe.
 * Returns number of bytes written.
 */
static int Pipe_Write(struct File *file, void *buf, ulong_t numBytes)
{
    TODO("Write to pipe");
}

/*
 * Close pipe.
 */
static int Pipe_Close(struct File *file)
{
    TODO("Close a pipe");
}

/*
 * Clone a pipe.
 */
static int Pipe_Clone(struct File *file, struct File **pClone)
{
    TODO("Clone a pipe");
}

static struct File_Ops s_readPipeFileOps = {
    0,			/* FStat() */
    &Pipe_Read,
    0,			/* Write() */
    0,			/* Seek() */
    &Pipe_Close,
    0,			/* Read_Entry() */
    &Pipe_Clone,
};

static struct File_Ops s_writePipeFileOps = {
    0,			/* FStat() */
    0,			/* Write() */
    &Pipe_Write,
    0,			/* Seek() */
    &Pipe_Close,
    0,			/* Read_Entry() */
    &Pipe_Clone,
};

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

int Create_Pipe(struct File **pRead, struct File **pWrite)
{
    int rc = 0;
    struct File *read = 0, *write = 0;

    /* Allocate File objects */
    if ((read = Allocate_File(&s_readPipeFileOps, 0, 0, 0, O_READ, 0)) == 0 ||
	(write = Allocate_File(&s_writePipeFileOps, 0, 0, 0, O_WRITE, 0)) == 0) {
	rc = ENOMEM;
	goto done;
    }

    /*
     * TODO: you should allocate a data structure for managing the
     * pipe, and store it in the fsData field of the read and
     * write pipes.  It will need a reference count field
     * in order to destroy it when the last File connected
     * to the pipe is closed.
     */

    *pRead = read;
    *pWrite = write;
    KASSERT(rc == 0);

done:
    if (rc != 0) {
	if (read != 0)
	    Free(read);
	if (write != 0)
	    Free(write);
    }
    return rc;
}

