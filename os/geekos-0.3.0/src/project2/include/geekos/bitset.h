/*
 * Bit set data structure
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_BITSET_H
#define GEEKOS_BITSET_H

#include <geekos/ktypes.h>

void* Create_Bit_Set(uint_t totalBits);
void Set_Bit(void *bitSet, uint_t bitPos);
void Clear_Bit(void *bitSet, uint_t bitPos);
bool Is_Bit_Set(void *bitSet, uint_t bitPos);
int Find_First_Free_Bit(void *bitSet, ulong_t totalBits);
int Find_First_N_Free(void *bitSet, uint_t runLength, ulong_t totalBits);
void Destroy_Bit_Set(void *bitSet);

#if 0
struct Bit_Set {
    int size;
    uchar_t bits[0];  /* Note: unwarranted chumminess with compiler */
};

struct Bit_Set *Create_Bit_Set(uchar_t *bits, int totalBits);
int Set_Bit(struct Bit_Set *set, int bitPos);
int Clear_Bit(struct Bit_Set *set, int bitPos);
int Is_Bit_Set(struct Bit_Set *set, int bitPos);
int Find_First_Free_Bit(struct Bit_Set *set);
int Find_First_N_Free(struct Bit_Set *set, int runLength);
uchar_t *Get_Bits(struct Bit_Set *set);
#endif

#endif
