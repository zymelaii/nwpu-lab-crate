/*
 * x86 port IO routines
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.9 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_IO_H
#define GEEKOS_IO_H

#include <geekos/ktypes.h>

void Out_Byte(ushort_t port, uchar_t value);
uchar_t In_Byte(ushort_t port);

void Out_Word(ushort_t port, ushort_t value);
ushort_t In_Word(ushort_t port);

void IO_Delay(void);

#endif  /* GEEKOS_IO_H */
