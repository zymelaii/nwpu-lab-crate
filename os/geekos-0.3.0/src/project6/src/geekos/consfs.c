/*
 * Console pseudo-filesystem
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/vfs.h>
#include <geekos/malloc.h>
#include <geekos/screen.h>
#include <geekos/keyboard.h>
#include <geekos/consfs.h>

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * Read from console (keyboard).
 * Returns number of bytes read, or error code.
 */
static int Console_Read(struct File *file, void *buf, ulong_t numBytes)
{
    TODO("Read from console");
}

/*
 * Write to console (screen).
 * Returns number of bytes written.
 */
static int Console_Write(struct File *file, void *buf, ulong_t numBytes)
{
    TODO("Write to console");
}

/*
 * Close console input or output file.
 * Returns 0 if successful, error code if not.
 */
static int Console_Close(struct File *file)
{
    TODO("Close console input or output");
}

/*
 * Clone a console File object.
 */
static int Console_Clone(struct File *file, struct File **pClone)
{
    struct File *clone;

    if (file->ops->Write == 0)
	clone = Open_Console_Input();
    else
	clone = Open_Console_Output();

    if (clone == 0)
	return ENOMEM;
    else {
	*pClone = clone;
	return 0;
    }
}

/*
 * File_Ops for console input.
 */
static struct File_Ops s_consInputFileOps = {
    0,			/* FStat() */
    &Console_Read,
    0,			/* Write() */
    0,			/* Seek() */
    &Console_Close,
    0,			/* Read_Entry() */
    &Console_Clone,
};

/*
 * File_Ops for console output.
 */
static struct File_Ops s_consOutputFileOps = {
    0,			/* FStat() */
    0,			/* Read() */
    &Console_Write,
    0,			/* Seek() */
    &Console_Close,
    0,			/* Read_Entry() */
    &Console_Clone,
};

/*
 * Create a File object for console input or output.
 * Returns null pointer if File can't be created.
 */
static struct File *Do_Open(struct File_Ops *ops, int mode)
{
    return Allocate_File(ops, 0, 0, 0, mode, 0);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Open a File which will read from the console (keyboard).
 */
struct File *Open_Console_Input(void)
{
    return Do_Open(&s_consInputFileOps, O_READ);
}

/*
 * Open a File which will write to the console (screen).
 */
struct File *Open_Console_Output(void)
{
    return Do_Open(&s_consOutputFileOps, O_WRITE);
}

