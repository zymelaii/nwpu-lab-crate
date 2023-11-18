/*
 * Paging (virtual memory) support
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.55 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/string.h>
#include <geekos/int.h>
#include <geekos/idt.h>
#include <geekos/kthread.h>
#include <geekos/kassert.h>
#include <geekos/screen.h>
#include <geekos/mem.h>
#include <geekos/malloc.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/user.h>
#include <geekos/vfs.h>
#include <geekos/crc32.h>
#include <geekos/paging.h>
#include <geekos/errno.h>
#include <geekos/bitset.h>
#include <geekos/blockdev.h>

/* ----------------------------------------------------------------------
 * Public data
 * ---------------------------------------------------------------------- */
pde_t                       *g_kernel_pde;
static void                 *Bitmap;
static struct Paging_Device *pagingDevice;
static int                   numPagingDiskPages;
/* ----------------------------------------------------------------------
 * Private functions/data
 * ---------------------------------------------------------------------- */

#define SECTORS_PER_PAGE (PAGE_SIZE / SECTOR_SIZE)

/*
 * flag to indicate if debugging paging code
 */
int debugFaults = 0;
#define Debug(args...) \
 if (debugFaults) Print(args)

void checkPaging() {
    unsigned long reg = 0;
    __asm__ __volatile__("movl %%cr0, %0" : "=a"(reg));
    Print("Paging on ? : %d\n", (reg & (1 << 31)) != 0);
}

void PrintNote(const char *msg) {
    int attr = Get_Current_Attr();
    Set_Current_Attr(ATTRIB(BLACK, AMBER | BRIGHT));
    Print("[NOTE] ");
    Set_Current_Attr(attr);
    Print(msg);
    Print("\n");
}

/*
 * Print diagnostic information for a page fault.
 */
static void Print_Fault_Info(uint_t address, faultcode_t faultCode) {
    extern uint_t g_freePageCount;

    Print(
        "Pid %d, Page Fault received, at address %x (%d pages free)\n",
        g_currentThread->pid,
        address,
        g_freePageCount);
    if (faultCode.protectionViolation)
        Print("   Protection Violation, ");
    else
        Print("   Non-present page, ");
    if (faultCode.writeFault)
        Print("Write Fault, ");
    else
        Print("Read Fault, ");
    if (faultCode.userModeFault)
        Print("in User Mode\n");
    else
        Print("in Supervisor Mode\n");
}

/*
 * Handler for page faults.
 * You should call the Install_Interrupt_Handler() function to
 * register this function as the handler for interrupt 14.
 */
void Page_Fault_Handler(struct Interrupt_State* state)
{
    KASSERT(!Interrupts_Enabled());
    ulong_t address = Get_Page_Fault_Address();
    faultcode_t faultCode = *((faultcode_t *) &(state->errorCode));
    struct User_Context *userContext = g_currentThread->userContext;
    if (faultCode.writeFault) {
        int res = Alloc_User_Page(userContext->pageDir, Round_Down_To_Page(address), PAGE_SIZE);
        if (res == -1) { Exit(-1); }
        return;
    } else {
        ulong_t page_dir_addr = address >> 22;
        ulong_t page_addr = (address << 10) >> 22;
        pde_t *page_dir_entry = (pde_t*)userContext->pageDir + page_dir_addr;
        pte_t *page_entry = NULL;
        if (page_dir_entry->present) {
            page_entry = (pte_t*)(page_dir_entry->pageTableBaseAddr << 12);
            page_entry += page_addr;
        } else {
            Print_Fault_Info(address, faultCode);
            Exit(-1);
        }
        if (page_entry->kernelInfo != KINFO_PAGE_ON_DISK) {
            Print_Fault_Info(address, faultCode);
            Exit(-1);
        }
        int pagefile_index = page_entry->pageBaseAddr;
        void *paddr = Alloc_Pageable_Page(page_entry, Round_Down_To_Page(address));
        if (paddr == NULL) { Exit(-1); }
        *(uint_t*)page_entry = 0;
        page_entry->present = 1;
        page_entry->flags = VM_WRITE | VM_READ | VM_USER;
        page_entry->globalPage = 0;
        page_entry->pageBaseAddr = (ulong_t)paddr >> 12;
        Enable_Interrupts();
        Read_From_Paging_File(paddr, Round_Down_To_Page(address), pagefile_index);
        Disable_Interrupts();
        Free_Space_On_Paging_File(pagefile_index);
        return;
    }
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize virtual memory by building page tables
 * for the kernel and physical memory.
 */
void Init_VM(struct Boot_Info *bootInfo) {
    /*
     * Hints:
     * - Build kernel page directory and page tables
     * - Call Enable_Paging() with the kernel page directory
     * - Install an interrupt handler for interrupt 14,
     *   page fault
     * - Do not map a page at address 0; this will help trap
     *   null pointer references
     */

    int    kernel_pde_entries;
    int    whole_pages;
    int    i, j;
    uint_t mem_addr;
    pte_t *cur_pte;

    whole_pages = bootInfo->memSizeKB / 4;

    kernel_pde_entries = whole_pages / NUM_PAGE_DIR_ENTRIES
                       + (whole_pages % NUM_PAGE_DIR_ENTRIES == 0 ? 0 : 1);
    g_kernel_pde = (pde_t *)Alloc_Page();
    KASSERT(g_kernel_pde != NULL);

    memset(g_kernel_pde, 0, PAGE_SIZE);

    pde_t *cur_pde_entry;
    cur_pde_entry = g_kernel_pde;
    mem_addr      = 0;
    for (i = 0; i < kernel_pde_entries - 1; i++) {
        cur_pde_entry->present = 1;
        cur_pde_entry->flags   = VM_WRITE;

        cur_pde_entry->globalPage = 1;

        cur_pte = (pte_t *)Alloc_Page();
        KASSERT(cur_pte != NULL);

        cur_pde_entry->present    = 1;
        cur_pde_entry->flags      = VM_WRITE;
        cur_pde_entry->globalPage = 1;
        cur_pte                   = (pte_t *)Alloc_Page();
        KASSERT(cur_pte != NULL);
        memset(cur_pte, 0, PAGE_SIZE);
        cur_pde_entry->pageTableBaseAddr = PAGE_ALLIGNED_ADDR(cur_pte);

        int last_pagetable_num;
        last_pagetable_num = whole_pages % NUM_PAGE_TABLE_ENTRIES;
        if (last_pagetable_num == 0) {
            last_pagetable_num = NUM_PAGE_TABLE_ENTRIES;
        }

        for (j = 0; j < last_pagetable_num; j++) {
            cur_pte->present      = 1;
            cur_pte->flags        = VM_WRITE;
            cur_pte->globalPage   = 1;
            cur_pte->pageBaseAddr = mem_addr >> 12;
            cur_pte++;
            mem_addr += PAGE_SIZE;
        }
        Enable_Paging(g_kernel_pde);

        Install_Interrupt_Handler(14, Page_Fault_Handler);
    }
}

/**
 * Initialize paging file data structures.
 * All filesystems should be mounted before this function
 * is called, to ensure that the paging file is available.
 */
void Init_Paging(void) {
    pagingDevice = Get_Paging_Device();
    if (pagingDevice == NULL) {
        Print("can not find pagefile\n");
        return;
    }
    numPagingDiskPages = pagingDevice->numSectors / SECTORS_PER_PAGE;

    Bitmap = Create_Bit_Set(numPagingDiskPages);
}

/**
 * Find a free bit of disk on the paging file for this page.
 * Interrupts must be disabled.
 * @return index of free page sized chunk of disk space in
 *   the paging file, or -1 if the paging file is full
 */
int Find_Space_On_Paging_File(void) {
    KASSERT(!Interrupts_Enabled());
    return Find_First_Free_Bit(Bitmap, numPagingDiskPages);
}

/**
 * Free a page-sized chunk of disk space in the paging file.
 * Interrupts must be disabled.
 * @param pagefileIndex index of the chunk of disk space
 */
void Free_Space_On_Paging_File(int pagefileIndex) {
    KASSERT(!Interrupts_Enabled());
    KASSERT(0 <= pagefileIndex && pagefileIndex < numPagingDiskPages);
    Clear_Bit(Bitmap, pagefileIndex);
}

/**
 * Write the contents of given page to the indicated block
 * of space in the paging file.
 * @param paddr a pointer to the physical memory of the page
 * @param vaddr virtual address where page is mapped in user memory
 * @param pagefileIndex the index of the page sized chunk of space
 *   in the paging file
 */
void Write_To_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex) {
    struct Page *page = Get_Page((ulong_t)paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); /* Page must be locked! */
    if (0 <= pagefileIndex && pagefileIndex < numPagingDiskPages) {
        int i;
        for (i = 0; i < SECTORS_PER_PAGE; i++) {
            Block_Write(
                pagingDevice->dev,
                pagefileIndex * SECTORS_PER_PAGE + i
                    + (pagingDevice->startSector),
                paddr + i * SECTOR_SIZE);
        }
        Set_Bit(Bitmap, pagefileIndex);
    } else {
        Exit(-1);
    }
}

/**
 * Read the contents of the indicated block
 * of space in the paging file into the given page.
 * @param paddr a pointer to the physical memory of the page
 * @param vaddr virtual address where page will be re-mapped in
 *   user memory
 * @param pagefileIndex the index of the page sized chunk of space
 *   in the paging file
 */
void Read_From_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex) {
    block_t     *block = (block_t *)paddr;
    struct Page *page  = Get_Page((ulong_t)paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); // Page must be locked!
    page->flags = page->flags & ~PAGE_PAGEABLE;
}
