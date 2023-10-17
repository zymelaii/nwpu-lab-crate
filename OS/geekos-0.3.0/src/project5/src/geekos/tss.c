/*
 * x86 TSS data structure and routines
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * Source: _Protected Mode Software Architecture_ by Tom Shanley,
 * ISBN 020155447X.
 */

#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/string.h>
#include <geekos/tss.h>

/*
 * We use one TSS in GeekOS.
 */
static struct TSS s_theTSS;
static struct Segment_Descriptor *s_tssDesc;
static ushort_t s_tssSelector;

static void __inline__ Load_Task_Register(void)
{
    /* Critical: TSS must be marked as not busy */
    s_tssDesc->type = 0x09;

    /* Load the task register */
    __asm__ __volatile__ (
	"ltr %0"
	:
	: "a" (s_tssSelector)
    );
}

/*
 * Initialize the kernel TSS.  This must be done after the memory and
 * GDT initialization, but before the scheduler is started.
 */
void Init_TSS(void)
{
    s_tssDesc = Allocate_Segment_Descriptor();
    KASSERT(s_tssDesc != 0);

    memset(&s_theTSS, '\0', sizeof(struct TSS));
    Init_TSS_Descriptor(s_tssDesc, &s_theTSS);

    s_tssSelector = Selector(0, true, Get_Descriptor_Index(s_tssDesc));

    Load_Task_Register();
}

/*
 * Set kernel stack pointer.
 * This should be called before switching to a new
 * user process, so that interrupts occurring while executing
 * in user mode will be delivered on the correct stack.
 */
void Set_Kernel_Stack_Pointer(ulong_t esp0)
{
    s_theTSS.ss0 = KERNEL_DS;
    s_theTSS.esp0 = esp0;

    /*
     * NOTE: I read on alt.os.development that it is necessary to
     * reload the task register after modifying a TSS.
     * I haven't verified this in the IA32 documentation,
     * but there is certainly no harm in being paranoid.
     */
    Load_Task_Register();
}
