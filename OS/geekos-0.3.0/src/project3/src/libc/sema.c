/*
 * Semaphores
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/syscall.h>
#include <string.h>
#include <sema.h>

DEF_SYSCALL(Create_Semaphore,SYS_CREATESEMAPHORE,int,(const char *name, int ival),
    const char *arg0 = name; size_t arg1 = strlen(name); int arg2 = ival;,
    SYSCALL_REGS_3)
DEF_SYSCALL(P,SYS_P,int,(int s),int arg0 = s;,SYSCALL_REGS_1)
DEF_SYSCALL(V,SYS_V,int,(int s),int arg0 = s;,SYSCALL_REGS_1)
DEF_SYSCALL(Destroy_Semaphore,SYS_DESTROYSEMAPHORE,int,(int s),int arg0 = s;,SYSCALL_REGS_1)

