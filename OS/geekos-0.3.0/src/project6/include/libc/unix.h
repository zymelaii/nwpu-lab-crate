/*
 * Emulation of Unix system calls and C library functions
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef UNIX_H
#define UNIX_H

#include <fileio.h>
#include <conio.h>

/*
 * This module contains Unix system calls and C library
 * functions needed for the ae editor.
 * FIXME: this file should be split up according to
 * POSIX and ISO C standards.
 */

extern int errno;

static __inline__ int isspace(int c) {
    return c == ' ' || c == '\f' ||  c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

#define printf(args...) Print(args)

int read(int fd, void *buf, size_t sz);
int write(int fd, void *buf, size_t sz);
int creat(const char *path, int flags);
int open(const char *path, int flags);
int close(int fd);
int exit(int exitCode);
const char *strerror(int errno);

#endif /* UNIX_H */

