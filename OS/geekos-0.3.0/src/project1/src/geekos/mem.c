/*
 * Physical memory allocation
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * $Revision: 1.44 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/defs.h>
#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/bootinfo.h>
#include <geekos/gdt.h>
#include <geekos/screen.h>
#include <geekos/int.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/mem.h>

/* ----------------------------------------------------------------------
 * Global data
 * ---------------------------------------------------------------------- */

/*
 * List of Page structures representing each page of physical memory.
 */
struct Page* g_pageList;

/*
 * Number of pages currently available on the freelist.
 */
uint_t g_freePageCount = 0;

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * Defined in paging.c
 */
extern int debugFaults;
#define Debug(args...) if (debugFaults) Print(args)

/*
 * List of pages available for allocation.
 */
static struct Page_List s_freeList;

/*
 * Total number of physical pages.
 */
int unsigned s_numPages;

/*
 * Add a range of pages to the inventory of physical memory.
 */
static void Add_Page_Range(ulong_t start, ulong_t end, int flags)
{
    ulong_t addr;

    KASSERT(Is_Page_Multiple(start));
    KASSERT(Is_Page_Multiple(end));
    KASSERT(start < end);

    for (addr = start; addr < end; addr += PAGE_SIZE) {
	struct Page *page = Get_Page(addr);

	page->flags = flags;

	if (flags == PAGE_AVAIL) {
	    /* Add the page to the freelist */
	    Add_To_Back_Of_Page_List(&s_freeList, page);

	    /* Update free page count */
	    ++g_freePageCount;
	} else {
	    Set_Next_In_Page_List(page, 0);
	    Set_Prev_In_Page_List(page, 0);
	}

    }
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * The linker defines this symbol to indicate the end of
 * the executable image.
 */
extern char end;

/*
 * Initialize memory management data structures.
 * Enables the use of Alloc_Page() and Free_Page() functions.
 */
void Init_Mem(struct Boot_Info* bootInfo)
{
    ulong_t numPages = bootInfo->memSizeKB >> 2;
    ulong_t endOfMem = numPages * PAGE_SIZE;
    unsigned numPageListBytes = sizeof(struct Page) * numPages;
    ulong_t pageListAddr;
    ulong_t kernEnd;

    KASSERT(bootInfo->memSizeKB > 0);

    /*
     * Before we do anything, switch from setup.asm's temporary GDT
     * to the kernel's permanent GDT.
     */
    Init_GDT();

    /*
     * We'll put the list of Page objects right after the end
     * of the kernel, and mark it as "kernel".  This will bootstrap
     * us sufficiently that we can start allocating pages and
     * keeping track of them.
     */
    pageListAddr = Round_Up_To_Page((ulong_t) &end);
    g_pageList = (struct Page*) pageListAddr;
    kernEnd = Round_Up_To_Page(pageListAddr + numPageListBytes);
    s_numPages = numPages;

    /*
     * The initial kernel thread and its stack are placed
     * just beyond the ISA hole.
     */
    KASSERT(ISA_HOLE_END == KERN_THREAD_OBJ);
    KASSERT(KERN_STACK == KERN_THREAD_OBJ + PAGE_SIZE);

    /*
     * Memory looks like this:
     * 0 - start: available (might want to preserve BIOS data area)
     * start - end: kernel
     * end - ISA_HOLE_START: available
     * ISA_HOLE_START - ISA_HOLE_END: used by hardware (and ROM BIOS?)
     * ISA_HOLE_END - HIGHMEM_START: used by initial kernel thread
     * HIGHMEM_START - end of memory: available
     *    (the kernel heap is located at HIGHMEM_START; any unused memory
     *    beyond that is added to the freelist)
     */

    Add_Page_Range(0, PAGE_SIZE, PAGE_UNUSED);
    Add_Page_Range(PAGE_SIZE, KERNEL_START_ADDR, PAGE_AVAIL);
    Add_Page_Range(KERNEL_START_ADDR, kernEnd, PAGE_KERN);
    Add_Page_Range(kernEnd, ISA_HOLE_START, PAGE_AVAIL);
    Add_Page_Range(ISA_HOLE_START, ISA_HOLE_END, PAGE_HW);
    Add_Page_Range(ISA_HOLE_END, HIGHMEM_START, PAGE_ALLOCATED);
    Add_Page_Range(HIGHMEM_START, HIGHMEM_START + KERNEL_HEAP_SIZE, PAGE_HEAP);
    Add_Page_Range(HIGHMEM_START + KERNEL_HEAP_SIZE, endOfMem, PAGE_AVAIL);

    /* Initialize the kernel heap */
    Init_Heap(HIGHMEM_START, KERNEL_HEAP_SIZE);

    Print("%uKB memory detected, %u pages in freelist, %d bytes in kernel heap\n",
	bootInfo->memSizeKB, g_freePageCount, KERNEL_HEAP_SIZE);
}

/*
 * Initialize the .bss section of the kernel executable image.
 */
void Init_BSS(void)
{
    extern char BSS_START, BSS_END;

    /* Fill .bss with zeroes */
    memset(&BSS_START, '\0', &BSS_END - &BSS_START);
}

/*
 * Allocate a page of physical memory.
 */
void* Alloc_Page(void)
{
    struct Page* page;
    void *result = 0;

    bool iflag = Begin_Int_Atomic();

    /* See if we have a free page */
    if (!Is_Page_List_Empty(&s_freeList)) {
	/* Remove the first page on the freelist. */
	page = Get_Front_Of_Page_List(&s_freeList);
	KASSERT((page->flags & PAGE_ALLOCATED) == 0);
	Remove_From_Front_Of_Page_List(&s_freeList);

	/* Mark page as having been allocated. */
	page->flags |= PAGE_ALLOCATED;
	g_freePageCount--;
	result = (void*) Get_Page_Address(page);
    }

    End_Int_Atomic(iflag);

    return result;
}

/*
 * Free a page of physical memory.
 */
void Free_Page(void* pageAddr)
{
    ulong_t addr = (ulong_t) pageAddr;
    struct Page* page;
    bool iflag;

    iflag = Begin_Int_Atomic();

    KASSERT(Is_Page_Multiple(addr));

    /* Get the Page object for this page */
    page = Get_Page(addr);
    KASSERT((page->flags & PAGE_ALLOCATED) != 0);

    /* Clear the allocation bit */
    page->flags &= ~(PAGE_ALLOCATED);

    /* Put the page back on the freelist */
    Add_To_Back_Of_Page_List(&s_freeList, page);
    g_freePageCount++;

    End_Int_Atomic(iflag);
}
