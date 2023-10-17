/*
 * User File I/O
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.22 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef FILEIO_H
#define FILEIO_H

#include <geekos/fileio.h>

int Stat(const char *path, struct VFS_File_Stat *stat);
int FStat(int fd, struct VFS_File_Stat *stat);
int Open(const char *path, int mode);
int Create_Directory(const char *path);
int Open_Directory(const char *path);
int Close(int fd);
int Read_Entry(int fd, struct VFS_Dir_Entry *dirEntry);
int Read(int fd, void *buf, unsigned long len);
int Write(int fd, const void *buf, unsigned long len);
int Sync(void);
int Format(const char *dev, const char *fstype);
int Mount(const char *dev, const char *prefix, const char *fstype);
int Seek(int fd, int pos);
int Delete(const char *path);

#endif  /* FILEIO_H */

