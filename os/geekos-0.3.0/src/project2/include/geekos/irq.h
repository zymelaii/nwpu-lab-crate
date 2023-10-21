/*
 * This is the device-driver interface to the interrupt system.
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_IRQ_H
#define GEEKOS_IRQ_H

#include <geekos/int.h>

void Install_IRQ(int irq, Interrupt_Handler handler);
ushort_t Get_IRQ_Mask(void);
void Set_IRQ_Mask(ushort_t mask);
void Enable_IRQ(int irq);
void Disable_IRQ(int irq);

/*
 * IRQ handlers should call these to begin and end the
 * interrupt.
 */
void Begin_IRQ(struct Interrupt_State* state);
void End_IRQ(struct Interrupt_State* state);

#endif  /* GEEKOS_IRQ_H */
