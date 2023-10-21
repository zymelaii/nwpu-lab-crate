/*
 * x86 TSS data structure and routines
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.10 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_TSS_H
#define GEEKOS_TSS_H

/*
 * Source: _Protected Mode Software Architecture_ by Tom Shanley,
 * ISBN 020155447X.
 */

/*
 * NOTE: all reserved fields must be set to zero.
 */

struct TSS {
    /*
     * Link to nested task.  For example, if an interrupt is handled
     * by a task gate, the link field will contain the selector for
     * the TSS of the interrupted task.
     */
    ushort_t link;
    ushort_t reserved1;

    /* Stacks for privilege levels.  esp0/ss0 specifies the kernel stack. */
    ulong_t esp0;
    ushort_t ss0;
    ushort_t reserved2;
    ulong_t esp1;
    ushort_t ss1;
    ushort_t reserved3;
    ulong_t esp2;
    ushort_t ss2;
    ushort_t reserved4;

    /* Page directory register. */
    ulong_t cr3;

    /* General processor registers. */
    ulong_t eip;
    ulong_t eflags;
    ulong_t eax;
    ulong_t ecx;
    ulong_t edx;
    ulong_t ebx;
    ulong_t esp;
    ulong_t ebp;
    ulong_t esi;
    ulong_t edi;

    /* Segment registers and padding. */
    ushort_t es;
    ushort_t reserved5;
    ushort_t cs;
    ushort_t reserved6;
    ushort_t ss;
    ushort_t reserved7;
    ushort_t ds;
    ushort_t reserved8;
    ushort_t fs;
    ushort_t reserved9;
    ushort_t gs;
    ushort_t reserved10;

    /* GDT selector for the LDT descriptor. */
    ushort_t ldt;
    ushort_t reserved11;

    /*
     * The debug trap bit causes a debug exception upon a switch
     * to the task specified by this TSS.
     */
    uint_t debugTrap : 1;
    uint_t reserved12 : 15;

    /* Offset in the TSS specifying where the io map is located. */
    ushort_t ioMapBase;
};

void Init_TSS(void);
void Set_Kernel_Stack_Pointer(ulong_t esp0);

#endif  /* GEEKOS_TSS_H */
