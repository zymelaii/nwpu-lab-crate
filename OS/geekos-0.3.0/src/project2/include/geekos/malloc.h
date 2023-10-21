/*
 * GeekOS memory allocation API
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.9 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_MALLOC_H
#define GEEKOS_MALLOC_H

#include <geekos/ktypes.h>

void Init_Heap(ulong_t start, ulong_t size);
void* Malloc(ulong_t size);
void Free(void* buf);

#endif  /* GEEKOS_MALLOC_H */
