/*
 * Initialize kernel GDT.
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.17 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/kassert.h>
#include <geekos/segment.h>
#include <geekos/int.h>
#include <geekos/tss.h>
#include <geekos/gdt.h>

/*
 * This is defined in lowlevel.asm.
 */
extern void Load_GDTR(ushort_t* limitAndBase);

/* ----------------------------------------------------------------------
 * Data
 * ---------------------------------------------------------------------- */

/*
 * Number of entries in the kernel GDT.
 */
#define NUM_GDT_ENTRIES 16

/*
 * This is the kernel's global descriptor table.
 */
static struct Segment_Descriptor s_GDT[ NUM_GDT_ENTRIES ];

/*
 * Number of allocated GDT entries.
 */
static int s_numAllocated = 0;

/* ----------------------------------------------------------------------
 * Functions
 * ---------------------------------------------------------------------- */

/*
 * Allocate an descriptor from the GDT.
 * Returns null if there are none left.
 */
struct Segment_Descriptor* Allocate_Segment_Descriptor(void)
{
    struct Segment_Descriptor* result = 0;
    int i;
    bool iflag;

    iflag = Begin_Int_Atomic();

    /* Note; entry 0 is unused (thus never allocated) */
    for (i = 1; i < NUM_GDT_ENTRIES; ++i) {
	struct Segment_Descriptor *desc = &s_GDT[ i ];
	if (desc->avail) {
	    ++s_numAllocated;
	    desc->avail = 0;
	    result = desc;
	    break;
	}
    }

    End_Int_Atomic(iflag);

    return result;
}

/*
 * Free a segment descriptor.
 */
void Free_Segment_Descriptor(struct Segment_Descriptor* desc)
{
    bool iflag = Begin_Int_Atomic();

    KASSERT(!desc->avail);

    Init_Null_Segment_Descriptor(desc);
    desc->avail = 1;
    --s_numAllocated;

    End_Int_Atomic(iflag);
}

/*
 * Get the index (int the GDT) of given segment descriptor.
 */
int Get_Descriptor_Index(struct Segment_Descriptor* desc)
{
    return (int) (desc - s_GDT);
}

/*
 * Initialize the kernel's GDT.
 */
void Init_GDT(void)
{
    ushort_t limitAndBase[3];
    ulong_t gdtBaseAddr = (ulong_t) s_GDT;
    struct Segment_Descriptor* desc;
    int i;

    KASSERT(sizeof(struct Segment_Descriptor) == 8);

    /* Clear out entries. */
    for (i = 0; i < NUM_GDT_ENTRIES; ++i) {
	desc = &s_GDT[ i ];
	Init_Null_Segment_Descriptor(desc);
	desc->avail = 1;
    }

    /* Kernel code segment. */
    desc = Allocate_Segment_Descriptor();
    Init_Code_Segment_Descriptor(
	desc,
	0,		 /* base address */
	0x100000,	 /* num pages (== 2^20) */
	0		 /* privilege level (0 == kernel) */
    );
    KASSERT(Get_Descriptor_Index(desc) == (KERNEL_CS >> 3));

    /* Kernel data segment. */
    desc = Allocate_Segment_Descriptor();
    Init_Data_Segment_Descriptor(
	desc,
	0,		 /* base address */
	0x100000,	 /* num pages (== 2^20) */
	0		 /* privilege level (0 == kernel) */
    );
    KASSERT(Get_Descriptor_Index(desc) == (KERNEL_DS >> 3));

    /* Activate the kernel GDT. */
    limitAndBase[0] = sizeof(struct Segment_Descriptor) * NUM_GDT_ENTRIES;
    limitAndBase[1] = gdtBaseAddr & 0xffff;
    limitAndBase[2] = gdtBaseAddr >> 16;
    Load_GDTR(limitAndBase);
}
