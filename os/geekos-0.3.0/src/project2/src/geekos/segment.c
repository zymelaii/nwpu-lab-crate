/*
 * General data structures and routines for segmentation
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.8 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * Source: _Protected Mode Software Architecture_ by Tom Shanley,
 * ISBN 020155447X.
 */

#include <geekos/kassert.h>
#include <geekos/string.h>
#include <geekos/tss.h>
#include <geekos/segment.h>

static __inline__ void Set_Size_And_Base_Pages(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numPages
)
{
    /*
     * There are 20 bits in the size fields of a segment descriptor.
     * The maximum possible value is thus 0xFFFFF, which in terms of
     * pages is one page less than 4GB.  So, I conclude that the
     * number of pages in the segment is one greater than the
     * value specified in the descriptor.
     */
    KASSERT(numPages > 0);
    numPages -= 1;

    desc->sizeLow     = numPages & 0xFFFF;
    desc->sizeHigh    = (numPages >> 16) & 0x0F;
    desc->baseLow     = baseAddr & 0xFFFFFF;
    desc->baseHigh    = (baseAddr >> 24) & 0xFF;
    desc->granularity = 1;  /* size in pages */
}

static __inline__ void Set_Size_And_Base_Bytes(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numBytes
)
{
    desc->sizeLow     = numBytes & 0xFFFF;
    desc->sizeHigh    = (numBytes >> 16) & 0x0F;
    desc->baseLow     = baseAddr & 0xFFFFFF;
    desc->baseHigh    = (baseAddr >> 24) & 0xFF;
    desc->granularity = 0;  /* size in bytes */
}

/*
 * Initialize an unused segment descriptor.
 */
void Init_Null_Segment_Descriptor(struct Segment_Descriptor* desc)
{
    memset(desc, '\0', sizeof(*desc));
}

/*
 * Initialize a code segment descriptor.
 */
void Init_Code_Segment_Descriptor(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numPages,
    int privilegeLevel
)
{
    KASSERT(privilegeLevel >= 0 && privilegeLevel <= 3);

    Set_Size_And_Base_Pages(desc, baseAddr, numPages);
    desc->type     = 0x0A;   /* 1010b: code, !conforming, readable, !accessed */
    desc->system   = 1;
    desc->dpl      = privilegeLevel;
    desc->present  = 1;
    desc->reserved = 0;
    desc->dbBit    = 1;  /* 32 bit code segment */
}

/*
 * Initialize a data segment descriptor.
 */
void Init_Data_Segment_Descriptor(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numPages,
    int privilegeLevel
)
{
    KASSERT(privilegeLevel >= 0 && privilegeLevel <= 3);

    Set_Size_And_Base_Pages(desc, baseAddr, numPages);
    desc->type     = 0x02;  /* 0010b: data, expand-up, writable, !accessed */
    desc->system   = 1;
    desc->dpl      = privilegeLevel;
    desc->present  = 1;
    desc->reserved = 0;
    desc->dbBit    = 1;  /* 32 bit operands */
}

/*
 * Initialize a TSS descriptor.
 */
void Init_TSS_Descriptor(struct Segment_Descriptor* desc, struct TSS* theTSS)
{
    Set_Size_And_Base_Bytes(desc, (ulong_t) theTSS, sizeof(struct TSS));
    desc->type     = 0x09;  /* 1001b: 32 bit, !busy */
    desc->system   = 0;
    desc->dpl      = 0;
    desc->present  = 1;
    desc->reserved = 0;
    desc->dbBit    = 0;  /* must be 0 in TSS */
}

/*
 * Initialize an LDT (Local Descriptor Table) descriptor.
 */
void Init_LDT_Descriptor(
    struct Segment_Descriptor* desc,
    struct Segment_Descriptor theLDT[],
    int numEntries
)
{
    Set_Size_And_Base_Bytes(
	desc, (ulong_t) theLDT, sizeof(struct Segment_Descriptor) * numEntries);

    desc->type     = 0x02;  /* 0010b */
    desc->system   = 0;
    desc->dpl      = 0;
    desc->present  = 1;
    desc->reserved = 0;
    desc->dbBit    = 0;
}
