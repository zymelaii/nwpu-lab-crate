/*
 * General data structures and routines for segmentation
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * Source: _Protected Mode Software Architecture_ by Tom Shanley,
 * ISBN 020155447X.
 */

#ifndef GEEKOS_SEGMENT_H
#define GEEKOS_SEGMENT_H

#include <geekos/ktypes.h>

struct TSS;

#if __TINYC__
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

/*
 * The general format of a segment descriptor.
 */
struct Segment_Descriptor {
    ushort_t sizeLow        PACKED ;
    uint_t baseLow     : 24 PACKED ;
    uint_t type        : 4  PACKED ;
    uint_t system      : 1  PACKED ;
    uint_t dpl         : 2  PACKED ;
    uint_t present     : 1  PACKED ;
    uint_t sizeHigh    : 4  PACKED ;
    uint_t avail       : 1  PACKED ;
    uint_t reserved    : 1  PACKED ;  /* set to zero */
    uint_t dbBit       : 1  PACKED ;
    uint_t granularity : 1  PACKED ;
    uchar_t baseHigh        PACKED ;
};

/**
 * Construct a segment selector.
 * @param rpl requestor privilege level; should be KERNEL_PRIVILEGE
 *    for kernel segments and USER_PRIVILEGE for user segments
 * @param segmentIsInGDT true if the referenced segment descriptor
 *    is defined in the GDT, false if it is defined in the LDT
 * @param index index of the segment descriptor
 * @return the segment selector
 */
static __inline__ ushort_t Selector(int rpl, bool segmentIsInGDT, int index)
{
    ushort_t selector = 0;
    selector = (rpl & 0x3) | ((segmentIsInGDT ? 0 : 1) << 2) | ((index & 0x1FFF) << 3);
    return selector;
}

/*
 * Routines to initialize segment descriptors.
 * Code and data segments must start on a page-aligned address
 * and are sized in pages.
 */

void Init_Null_Segment_Descriptor(struct Segment_Descriptor* desc);

void Init_Code_Segment_Descriptor(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numPages,
    int privilegeLevel
);
void Init_Data_Segment_Descriptor(
    struct Segment_Descriptor* desc,
    ulong_t baseAddr,
    ulong_t numPages,
    int privilegeLevel
);
void Init_TSS_Descriptor(struct Segment_Descriptor* desc, struct TSS* theTSS);

void Init_LDT_Descriptor(
    struct Segment_Descriptor* desc,
    struct Segment_Descriptor theLDT[],
    int numEntries
);

#endif  /* GEEKOS_SEGMENT_H */
