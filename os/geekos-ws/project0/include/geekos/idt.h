/*
 * GeekOS IDT initialization code
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.10 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_IDT_H
#define GEEKOS_IDT_H

#include <geekos/int.h>

/*
 * We'll handle all possible interrupts.
 */
#define NUM_IDT_ENTRIES 256

/*
 * Exceptions range from 0-17
 */
#define FIRST_EXCEPTION 0
#define NUM_EXCEPTIONS 18

/*
 * External IRQs range from 32-47
 */
#define FIRST_EXTERNAL_INT 32
#define NUM_EXTERNAL_INTS 16

struct Interrupt_Gate {
    ushort_t offsetLow;
    ushort_t segmentSelector;
    unsigned reserved : 5;
    unsigned signature : 8;
    unsigned dpl : 2;
    unsigned present : 1;
    ushort_t offsetHigh;
};

union IDT_Descriptor {
    struct Interrupt_Gate ig;
    /*
     * In theory we could have members for trap gates
     * and task gates if we wanted.
     */
};

void Init_IDT(void);
void Init_Interrupt_Gate(union IDT_Descriptor* desc, ulong_t addr,
	int dpl);
void Install_Interrupt_Handler(int interrupt, Interrupt_Handler handler);

/*
 * This is defined in lowlevel.asm.
 * The parameter should consist of 16 bit base,
 * followed by 32 bit base address, describing the IDT.
 */
void Load_IDTR(ushort_t* limitAndBase);

#endif  /* GEEKOS_IDT_H */
