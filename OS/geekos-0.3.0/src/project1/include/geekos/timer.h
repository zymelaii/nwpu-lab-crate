/*
 * GeekOS timer interrupt support
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_TIMER_H
#define GEEKOS_TIMER_H

#define TIMER_IRQ 0

extern volatile ulong_t g_numTicks;

void Init_Timer(void);

void Micro_Delay(int us);


void Micro_Delay(int us);

#endif  /* GEEKOS_TIMER_H */
