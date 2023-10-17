/*
 * Emulation of Unix system calls and C library functions
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>
#include <unix.h>

int errno;

static int Wrap(int rc)
{
    if (rc < 0)
	errno = rc;
    else
	errno = 0;
    return rc;
}

int read(int fd, void *buf, size_t sz)
{
    return Wrap(Read(fd, buf, sz));
}

int write(int fd, void *buf, size_t sz)
{
    return Wrap(Write(fd, buf, sz));
}

int creat(const char *path, int flags)
{
    return Wrap(Open(path, flags|O_CREATE));
}

int open(const char *path, int flags)
{
    return Wrap(Open(path, flags|O_CREATE));
}

int close(int fd)
{
    return Wrap(Close(fd));
}

int exit(int exitCode)
{
    Exit(exitCode);
    while (1);
}

const char *strerror(int errc)
{
    return Get_Error_String(errc);
}
