/*
 * Physical memory allocation
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * $Revision: 1.36 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_MEM_H
#define GEEKOS_MEM_H

#include <geekos/ktypes.h>
#include <geekos/defs.h>
#include <geekos/list.h>

struct Boot_Info;

/*
 * Page flags
 */
#define PAGE_AVAIL     0x0000	 /* page is on the freelist */
#define PAGE_KERN      0x0001	 /* page used by kernel code or data */
#define PAGE_HW        0x0002	 /* page used by hardware (e.g., ISA hole) */
#define PAGE_ALLOCATED 0x0004	 /* page is allocated */
#define PAGE_UNUSED    0x0008	 /* page is unused */
#define PAGE_HEAP      0x0010	 /* page is in kernel heap */

/*
 * PC memory map
 */
#define ISA_HOLE_START 0x0A0000
#define ISA_HOLE_END   0x100000

/*
 * We reserve the two pages just after the ISA hole for the initial
 * kernel thread's context object and stack.
 */
#define HIGHMEM_START (ISA_HOLE_END + 8192)

/*
 * Make the kernel heap this size
 */
#define KERNEL_HEAP_SIZE (1024*1024)

struct Page;

/*
 * List datatype for doubly-linked list of Pages.
 */
DEFINE_LIST(Page_List, Page);

/*
 * Each page of physical memory has one of these structures
 * associated with it, to do allocation and bookkeeping.
 */
struct Page {
    unsigned flags;			 /* Flags indicating state of page */
    DEFINE_LINK(Page_List, Page);	 /* Link fields for Page_List */
};

IMPLEMENT_LIST(Page_List, Page);

void Init_Mem(struct Boot_Info* bootInfo);
void Init_BSS(void);
void* Alloc_Page(void);
void Free_Page(void* pageAddr);

/*
 * Determine if given address is a multiple of the page size.
 */
static __inline__ bool Is_Page_Multiple(ulong_t addr)
{
    return addr == (addr & ~(PAGE_MASK));
}

/*
 * Round given address up to a multiple of the page size
 */
static __inline__ ulong_t Round_Up_To_Page(ulong_t addr)
{
    if ((addr & PAGE_MASK) != 0) {
	addr &= ~(PAGE_MASK);
	addr += PAGE_SIZE;
    }
    return addr;
}

/*
 * Round given address down to a multiple of the page size
 */
static __inline__ ulong_t Round_Down_To_Page(ulong_t addr)
{
    return addr & (~PAGE_MASK);
}

/*
 * Get the index of the page in memory.
 */
static __inline__ int Page_Index(ulong_t addr)
{
    return (int) (addr >> PAGE_POWER);
}

/*
 * Get the Page struct associated with given address.
 */
static __inline__ struct Page *Get_Page(ulong_t addr)
{
    extern struct Page* g_pageList;
    return &g_pageList[Page_Index(addr)];
}

/*
 * Get the physical address of the memory represented by given Page object.
 */
static __inline__ ulong_t Get_Page_Address(struct Page *page)
{
    extern struct Page* g_pageList;
    ulong_t index = page - g_pageList;
    return index << PAGE_POWER;
}

#endif  /* GEEKOS_MEM_H */
