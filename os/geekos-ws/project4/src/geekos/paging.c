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
#include <geekos/bitset.h>

/* ----------------------------------------------------------------------
 * Public data
 * ---------------------------------------------------------------------- */

pde_t *g_kernel_pde = NULL;
void* bitmapPaging = NULL;
struct Paging_Device *pagingDevice;
static int numOfPagingPages = 0;

/* ----------------------------------------------------------------------
 * Private functions/data
 * ---------------------------------------------------------------------- */

#define SECTORS_PER_PAGE (PAGE_SIZE / SECTOR_SIZE)

/*
 * flag to indicate if debugging paging code
 */
int debugFaults = 0;
#define Debug(args...) if (debugFaults) Print(args)


void checkPaging()
{
  unsigned long reg=0;
  __asm__ __volatile__( "movl %%cr0, %0" : "=a" (reg));
  Print("Paging on ? : %d\n", (reg & (1<<31)) != 0);
}


/*
 * Print diagnostic information for a page fault.
 */
static void Print_Fault_Info(uint_t address, faultcode_t faultCode)
{
    extern uint_t g_freePageCount;

    Print("Pid %d, Page Fault received, at address %x (%d pages free)\n",
        g_currentThread->pid, address, g_freePageCount);
    if (faultCode.protectionViolation)
        Print ("   Protection Violation, ");
    else
        Print ("   Non-present page, ");
    if (faultCode.writeFault)
        Print ("Write Fault, ");
    else
        Print ("Read Fault, ");
    if (faultCode.userModeFault)
        Print ("in User Mode\n");
    else
        Print ("in Supervisor Mode\n");
}

/*
 * Handler for page faults.
 * You should call the Install_Interrupt_Handler() function to
 * register this function as the handler for interrupt 14.
 */
/*static*/ void Page_Fault_Handler(struct Interrupt_State* state)
{
    ulong_t address;
    faultcode_t faultCode;

    KASSERT(!Interrupts_Enabled());

    /* Get the address that caused the page fault */
    address = Get_Page_Fault_Address();
    Debug("Page fault @%lx\n", address);

    /* Get the fault code */
    faultCode = *((faultcode_t *) &(state->errorCode));

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

uint_t lin_to_phyaddr(pde_t *page_dir, uint_t lin_addr)
{
    uint_t dir_index = lin_addr >> 22;
    uint_t page_index = (lin_addr << 10) >> 22;
    uint_t offset = lin_addr & 0xfff;
    pde_t *pagedir_entry = page_dir + dir_index;
    if (pagedir_entry->present) {
        pte_t *page_entry = (pte_t*)((uint_t)pagedir_entry->pageTableBaseAddr << 12);
        page_entry += page_index;
        return (page_entry->pageBaseAddr << 12) + offset;
    } else {
        return 0;
    }
}

int Alloc_User_Page(pde_t *pageDir, uint_t startAddress, uint_t sizeInMemory)
{
    uint_t dirindex = startAddress >> 22;
    uint_t pageindex = (startAddress << 10) >> 22;
    pde_t *pagedir_entry = pageDir + dirindex;
    pte_t *page_entry = NULL;

    if (pagedir_entry->present) {
        page_entry = (pte_t*)(pagedir_entry->pageTableBaseAddr << 12);
    } else {
        page_entry = (pte_t*)Alloc_Page();
        if (page_entry == NULL) { return -1; }
        memset(page_entry, 0, PAGE_SIZE);
        *(uint_t*)pagedir_entry = 0;
        pagedir_entry->present = 1;
        pagedir_entry->flags = VM_WRITE | VM_READ | VM_USER;
        pagedir_entry->pageTableBaseAddr = (ulong_t)page_entry >> 12;
    }

    page_entry += pageindex;
    void *page_addr = NULL;
    int num_pages = Round_Up_To_Page(startAddress - Round_Down_To_Page(startAddress) + sizeInMemory) / PAGE_SIZE;
    uint_t first_page_addr = 0;
    for (int i = 0; i < num_pages; ++i) {
        if (!page_entry->present) {
            page_addr = Alloc_Pageable_Page(page_entry, Round_Down_To_Page(startAddress));
            if (page_addr == NULL) { return -1; }
            *(uint_t*)page_entry = 0;
            page_entry->present = 1;
            page_entry->flags = VM_WRITE | VM_READ | VM_USER;
            page_entry->globalPage = 0;
            page_entry->pageBaseAddr = (ulong_t)page_addr >> 12;
            KASSERT(page_addr != 0);
            if (i == 0) {
                first_page_addr = (uint_t)page_addr;
            }
        }
        ++page_entry;
        startAddress += PAGE_SIZE;
    }
    return 0;
}

void Free_User_Pages(struct User_Context *context)
{
    pde_t *dir = context->pageDir;
    for (int i = 512; i < 1019; ++i) { //<! 1019 -> APIC
        pde_t *pde = dir + i;
        if (pde->present) {
            pte_t *table = (pte_t*)(pde->pageTableBaseAddr << 12);
            for (int j = 0; j < 1024; ++j) {
                pte_t *pte = table + j;
                if (pte->present) {
                    if (pte->kernelInfo != KINFO_PAGE_ON_DISK) {
                        Free_Page((void*)((uint_t)pte->pageBaseAddr << 12));
                    } else {
                        Free_Space_On_Paging_File(pte->pageBaseAddr);
                    }
                }
            }
            Free_Page(table);
        }
    }
    Free_Page(dir);
}

bool Copy_User_Page(pde_t *pagedir, uint_t user_dest, char *src, uint_t bytes)
{
    uint_t tmplen = 0;
    int page_nums = 0;
    if (Round_Down_To_Page(user_dest + bytes) == Round_Down_To_Page(user_dest)) {
        tmplen = bytes;
        page_nums = 1;
    } else {
        tmplen = Round_Up_To_Page(user_dest) - user_dest;
        bytes -= tmplen;
        page_nums = 0;
    }

    uint_t phy_start = lin_to_phyaddr(pagedir, user_dest);
    if (phy_start == 0) { return false; }
    struct Page *page = Get_Page(phy_start);
    Disable_Interrupts();
    page->flags &= ~PAGE_PAGEABLE;
    Enable_Interrupts();
    memcpy((void*)phy_start, src, tmplen);
    page->flags |= PAGE_PAGEABLE;
    if (page_nums == 1) { return true; }
    tmplen = Round_Up_To_Page(user_dest) - user_dest;
    user_dest += tmplen;
    src += tmplen;
    bytes -= tmplen;

    while (user_dest != Round_Down_To_Page(user_dest + bytes)) {
        phy_start = lin_to_phyaddr(pagedir, user_dest);
        if (phy_start == 0) { return false; }
        page = Get_Page(phy_start);
        Disable_Interrupts();
        page->flags &= ~PAGE_PAGEABLE;
        Enable_Interrupts();
        memcpy((void*)phy_start, src, PAGE_SIZE);
        page->flags |= PAGE_PAGEABLE;
        user_dest += PAGE_SIZE;
        src += PAGE_SIZE;
        bytes -= PAGE_SIZE;
    }

    phy_start = lin_to_phyaddr(pagedir, user_dest);
    if (phy_start == 0) { return false; }
    page = Get_Page(phy_start);
    Disable_Interrupts();
    page->flags &= ~PAGE_PAGEABLE;
    Enable_Interrupts();
    memcpy((void*)phy_start, src, bytes);
    page->flags |= PAGE_PAGEABLE;
    return true;
}

/*
 * Initialize virtual memory by building page tables
 * for the kernel and physical memory.
 */
void Init_VM(struct Boot_Info *bootInfo)
{
    /*
     * Hints:
     * - Build kernel page directory and page tables
     * - Call Enable_Paging() with the kernel page directory
     * - Install an interrupt handler for interrupt 14,
     *   page fault
     * - Do not map a page at address 0; this will help trap
     *   null pointer references
     */

    int num_dir_entries = (bootInfo->memSizeKB / 4) / NUM_PAGE_TABLE_ENTRIES + 1;
    g_kernel_pde = Alloc_Page();
    KASSERT(g_kernel_pde != NULL);
    memset(g_kernel_pde, 0, PAGE_SIZE);
    for (int i = 0; i < num_dir_entries; ++i) {
        g_kernel_pde[i].flags = VM_WRITE | VM_USER;
        g_kernel_pde[i].present = 1;
        pte_t *first_pte = Alloc_Page();
        KASSERT(first_pte != NULL);
        memset(first_pte, 0, PAGE_SIZE);
        g_kernel_pde[i].pageTableBaseAddr = ((uint_t)first_pte) >> 12;
        uint_t mem = i * NUM_PAGE_TABLE_ENTRIES * PAGE_SIZE;
        for (int j = 0; j < NUM_PAGE_TABLE_ENTRIES; ++j) {
            first_pte[j].flags = VM_WRITE;
            first_pte[j].present = 1;
            first_pte[j].pageBaseAddr = mem >> 12;
            mem += PAGE_SIZE;
        }
    }
    int i = 1019;
    g_kernel_pde[i].present = 1;
    g_kernel_pde[i].flags = VM_WRITE;
    pte_t *first_pte = Alloc_Page();
    KASSERT(first_pte != NULL);
    memset(first_pte, 0, PAGE_SIZE);
    g_kernel_pde[i].pageTableBaseAddr = ((uint_t)first_pte) >> 12;
    uint_t mem = i * NUM_PAGE_TABLE_ENTRIES * PAGE_SIZE;
    for (int j = 0; j < NUM_PAGE_TABLE_ENTRIES; ++j) {
        first_pte[j].flags = VM_WRITE;
        first_pte[j].present = 1;
        first_pte[j].pageBaseAddr = mem >> 12;
        mem += PAGE_SIZE;
    }
    Enable_Paging(g_kernel_pde);
    Install_Interrupt_Handler(14, Page_Fault_Handler);
    Install_Interrupt_Handler(16, Page_Fault_Handler);
}

/**
 * Initialize paging file data structures.
 * All filesystems should be mounted before this function
 * is called, to ensure that the paging file is available.
 */
void Init_Paging(void)
{
    pagingDevice = Get_Paging_Device();
    KASSERT(pagingDevice != NULL);
    numOfPagingPages = pagingDevice->numSectors / SECTORS_PER_PAGE;
    bitmapPaging = Create_Bit_Set(numOfPagingPages);
}

/**
 * Find a free bit of disk on the paging file for this page.
 * Interrupts must be disabled.
 * @return index of free page sized chunk of disk space in
 *   the paging file, or -1 if the paging file is full
 */
int Find_Space_On_Paging_File(void)
{
    KASSERT(!Interrupts_Enabled());
    return Find_First_Free_Bit(bitmapPaging, numOfPagingPages);
}

/**
 * Free a page-sized chunk of disk space in the paging file.
 * Interrupts must be disabled.
 * @param pagefileIndex index of the chunk of disk space
 */
void Free_Space_On_Paging_File(int pagefileIndex)
{
    KASSERT(!Interrupts_Enabled());
    KASSERT(pagefileIndex >= 0 && pagefileIndex < numOfPagingPages);
    Clear_Bit(bitmapPaging, pagefileIndex);
}

/**
 * Write the contents of given page to the indicated block
 * of space in the paging file.
 * @param paddr a pointer to the physical memory of the page
 * @param vaddr virtual address where page is mapped in user memory
 * @param pagefileIndex the index of the page sized chunk of space
 *   in the paging file
 */
void Write_To_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex)
{
    struct Page *page = Get_Page((ulong_t) paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); /* Page must be locked! */
    KASSERT(page->flags & PAGE_LOCKED);
    KASSERT(pagefileIndex >= 0 && pagefileIndex < numOfPagingPages);
    for (int i = 0; i < SECTORS_PER_PAGE; ++i) {
        int blockNum = pagefileIndex * SECTORS_PER_PAGE + i + pagingDevice->startSector;
        Block_Write(pagingDevice->dev, blockNum, paddr + i * SECTOR_SIZE);
    }
    Set_Bit(bitmapPaging, pagefileIndex);
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
void Read_From_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex)
{
    struct Page *page = Get_Page((ulong_t) paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); /* Page must be locked! */
    KASSERT(page->flags & PAGE_LOCKED);
    KASSERT(pagefileIndex >= 0 && pagefileIndex < numOfPagingPages);
    for (int i = 0; i < SECTORS_PER_PAGE; ++i) {
        int blockNum = pagefileIndex * SECTORS_PER_PAGE + i + pagingDevice->startSector;
        Block_Read(pagingDevice->dev, blockNum, paddr + i * SECTOR_SIZE);
    }
}
