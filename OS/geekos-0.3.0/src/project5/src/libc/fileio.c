/*
 * User File I/O
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.28 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/syscall.h>
#include <fileio.h>
#include <string.h>

DEF_SYSCALL(Stat,SYS_STAT,int, (const char *filename, struct VFS_File_Stat *stat),
    const char *arg0 = filename; size_t arg1 = strlen(filename); struct VFS_File_Stat *arg2 = stat;,
    SYSCALL_REGS_3)
DEF_SYSCALL(FStat,SYS_FSTAT,int, (int fd, struct VFS_File_Stat *stat),
    int arg0 = fd; struct VFS_File_Stat *arg1 = stat;,
    SYSCALL_REGS_2)
DEF_SYSCALL(Open,SYS_OPEN,int, (const char *filename, int mode),
    const char *arg0 = filename; size_t arg1 = strlen(filename); int arg2 = mode;,
    SYSCALL_REGS_3)
DEF_SYSCALL(Create_Directory,SYS_CREATEDIR,int, (const char *dirname),
    const char *arg0 = dirname; size_t arg1 = strlen(dirname);,
    SYSCALL_REGS_2)
DEF_SYSCALL(Open_Directory,SYS_OPENDIRECTORY,int, (const char *dirname),
    const char *arg0 = dirname; size_t arg1 = strlen(dirname);,
    SYSCALL_REGS_2)
DEF_SYSCALL(Close,SYS_CLOSE,int, (int fd), int arg0 = fd;, SYSCALL_REGS_1)
DEF_SYSCALL(Read_Entry,SYS_READENTRY,int, (int fd, struct VFS_Dir_Entry *entry),
    int arg0 = fd; struct VFS_Dir_Entry *arg1 = entry;,
    SYSCALL_REGS_2)
DEF_SYSCALL(Read,SYS_READ,int, (int fd, void *buf, ulong_t len),
    int arg0 = fd; void *arg1 = buf; ulong_t arg2 = len;,
    SYSCALL_REGS_3)
DEF_SYSCALL(Write,SYS_WRITE,int, (int fd, const void *buf, ulong_t len),
    int arg0 = fd; const void *arg1 = buf; ulong_t arg2 = len;,
    SYSCALL_REGS_3)
DEF_SYSCALL(Sync,SYS_SYNC,int,(void),,SYSCALL_REGS_0)
DEF_SYSCALL(Format,SYS_FORMAT,int,(const char *devname, const char *fstype),
    const char *arg0 = devname; size_t arg1 = strlen(devname); const char *arg2 = fstype; size_t arg3 = strlen(fstype);,
    SYSCALL_REGS_4)
DEF_SYSCALL(Seek,SYS_SEEK,int,(int fd, int pos),
    int arg0 = fd; int arg1 = pos;,
    SYSCALL_REGS_2)
DEF_SYSCALL(Delete,SYS_DELETE,int,(const char *path),
    const char *arg0 = path; size_t arg1 = strlen(path);,
    SYSCALL_REGS_2)



static bool Copy_String(char *dst, const char *src, size_t len)
{
    if (strnlen(src, len) == len)
	return false;
    strcpy(dst, src);
    return true;
}

/*
 * The Mount() system call requires special handling because
 * its arguments are passed in a struct, since too many registers
 * would be required to pass the arguments entirely in
 * registers.
 */
int Mount(const char *devname, const char *prefix, const char *fstype)
{
    int num = SYS_MOUNT, rc;
    struct VFS_Mount_Request args;

    if (!Copy_String(args.devname, devname, sizeof(args.devname)) ||
	!Copy_String(args.prefix, prefix, sizeof(args.prefix)) ||
	!Copy_String(args.fstype, fstype, sizeof(args.fstype)))
	return EINVALID;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (&args)
    );
    return rc;
}

