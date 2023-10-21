/*
 * Trap handlers
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.20 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/idt.h>
#include <geekos/kthread.h>
#include <geekos/defs.h>
#include <geekos/syscall.h>
#include <geekos/trap.h>

/*
 * TODO: need to add handlers for other exceptions (such as bounds
 * check, debug, etc.)
 */

/*
 * Handler for general protection faults and other bad errors.
 * Kill the current thread (which caused the fault).
 */
static void GPF_Handler(struct Interrupt_State* state)
{
    /* Send the thread to the reaper... */
    Print("Exception %d received, killing thread %p\n",
	state->intNum, g_currentThread);
    Dump_Interrupt_State(state);

    Exit(-1);

    /* We will never get here */
    KASSERT(false);
}

/*
 * System call handler.
 */
static void Syscall_Handler(struct Interrupt_State* state)
{
    /* The system call number is specified in the eax register. */
    uint_t syscallNum = state->eax;

    /* Make sure the the system call number refers to a legal value. */
    if (syscallNum < 0 || syscallNum >= g_numSyscalls) {
	Print("Illegal system call %d by process %d\n",
		syscallNum, g_currentThread->pid);
	Exit(-1);

	/* We will never get here */
	KASSERT(false);
    }

    /*
     * Call the appropriate syscall function.
     * Return code of system call is returned in EAX.
     */
    state->eax = g_syscallTable[syscallNum](state);
}

/*
 * Initialize handlers for processor traps.
 */
void Init_Traps(void)
{
    Install_Interrupt_Handler(12, &GPF_Handler);  /* stack exception */
    Install_Interrupt_Handler(13, &GPF_Handler);  /* general protection fault */
    Install_Interrupt_Handler(SYSCALL_INT, &Syscall_Handler);
}
