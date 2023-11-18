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
pde_t *g_kernel_pde;
static void *Bitmap;
static struct Paging_Device *pagingDevice;
static int numPagingDiskPages;
/* ----------------------------------------------------------------------
 * Private functions/data
 * ---------------------------------------------------------------------- */

#define SECTORS_PER_PAGE (PAGE_SIZE / SECTOR_SIZE)

/*
 * flag to indicate if debugging paging code
 */
int debugFaults = 0;
#define Debug(args...) \
    if (debugFaults)   \
    Print(args)

void checkPaging()
{
    unsigned long reg = 0;
    __asm__ __volatile__("movl %%cr0, %0"
                         : "=a"(reg));
    Print("Paging on ? : %d\n", (reg & (1 << 31)) != 0);
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
/*static*/ void Page_Fault_Handler(struct Interrupt_State *state)
{
    ulong_t address;
    faultcode_t faultCode;

    KASSERT(!Interrupts_Enabled());

    /* Get the address that caused the page fault */
    address = Get_Page_Fault_Address();
    // Print("Page fault @%lx\n", address);

    /* Get the fault code */
    faultCode = *((faultcode_t *)&(state->errorCode));

    /* rest of your handling code here */
    // Print("Unexpected Page Fault received\n");
    // Print_Fault_Info(address, faultCode);
    // Dump_Interrupt_State(state);
    // /* user faults just kill the process */
    struct User_Context *userContext = g_currentThread->userContext;

    //写错误，缺页情况为堆栈生长到新页
    if (faultCode.writeFault)
    {
        Print("write Fault\n");
        int res;
        if (!Alloc_User_Page(userContext->pageDir, Round_Down_To_Page(address), PAGE_SIZE))
        {
            Print("Alloc_User_Page error in Page_Fault_Handler\n");
            Exit(-1);
        }
        return;
    }
    else
    {
        //读错误，分两种缺页情况
        Print("read fault\n");
        //先找到虚拟地址对应的页表项
        ulong_t page_dir_addr = PAGE_DIRECTORY_INDEX(address);
        ulong_t page_addr = (address << 10) >> 22;
        pde_t *page_dir_entry = (pde_t *)userContext->pageDir + page_dir_addr;
        pte_t *page_entry = NULL;

        // Print("address=%x\n", address);
        // Print("userContext->pageDir=%x\n", userContext->pageDir);
        // Print("page_dir_entry=%x\n", page_dir_entry);

        if (page_dir_entry->present)
        {
            // Print("page_dir_entry->present=%x\n", page_dir_entry->present);
            page_entry = (pte_t *)((page_dir_entry->pageTableBaseAddr) << 12);
            page_entry += page_addr;

            // Print("page_entry=%x\n", page_entry);
            // Print("*page_entry=%x\n", *page_entry);
            // Print("page_entry->present=%x\n", page_entry->present);
            // Print("page_entry->pageBaseAddr=%x\n", (page_entry->pageBaseAddr) << 12);
        }
        else
        {
            //非法地址访问的缺页情况
            Print_Fault_Info(address, faultCode);
            Exit(-1);
        }

        if (page_entry->kernelInfo != KINFO_PAGE_ON_DISK)
        {
            // Print("page_entry->kernelInfo=%x\n", page_entry->kernelInfo);
            //非法地址访问的缺页情况
            Print_Fault_Info(address, faultCode);
            Exit(-1);
        }
        //因为页保存在磁盘pagefile引起的缺页
        int pagefile_index = page_entry->pageBaseAddr;
        void *paddr = Alloc_Pageable_Page(page_entry, Round_Down_To_Page(address));
        if (paddr == NULL)
        {
            Print("no more page/n");
            Exit(-1);
        }

        *((uint_t *)page_entry) = 0;
        page_entry->present = 1;
        page_entry->flags = VM_WRITE | VM_READ | VM_USER;
        page_entry->globalPage = 0;
        page_entry->pageBaseAddr = PAGE_ALLIGNED_ADDR(paddr);
        //从页面文件中把页读到内存中
        Enable_Interrupts();
        Read_From_Paging_File(paddr, Round_Down_To_Page(address), pagefile_index);
        Disable_Interrupts();
        //释放页面文件中的空间
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
    // TODO("Build initial kernel page directory and page tables");
    int kernel_pde_entries;
    int whole_pages;
    int i, j;
    uint_t mem_addr;
    pte_t *cur_pte;
    //计算物理内存的页数
    whole_pages = bootInfo->memSizeKB / 4;

    //计算内核页目录中要多少个目录项，才能完全映射所有的物理内存页。
    kernel_pde_entries = whole_pages / NUM_PAGE_DIR_ENTRIES + (whole_pages % NUM_PAGE_DIR_ENTRIES == 0 ? 0 : 1);

    //为内核页目录分配一页空间
    g_kernel_pde = (pde_t *)Alloc_Page();
    KASSERT(g_kernel_pde != NULL);

    //将页中所有位清0
    memset(g_kernel_pde, 0, PAGE_SIZE);

    pde_t *cur_pde_entry;
    cur_pde_entry = g_kernel_pde;
    mem_addr = 0;
    for (i = 0; i < kernel_pde_entries - 1; i++)
    {
        cur_pde_entry->present = 1;
        cur_pde_entry->flags = VM_WRITE;
        //置为全局页，当一个页被标明为全局的，并且CR4中的启用全局页标志（PGE）被置位时，
        //一旦CR3寄存器被载入或发生任务切换（此时CR3中的值会改变），TLB中的页表或指向页的页目录表项并不失效。
        //这个标志可以防止使TLB中频繁使用的页（比如操作系统内核或其他系统代码）失效。
        //注：必须先启用分页机制（通过设置CR0中的PG标志），再启用CR4中的PGE标志，才能启用全局页特性
        cur_pde_entry->globalPage = 1;

        cur_pte = (pte_t *)Alloc_Page();
        KASSERT(cur_pte != NULL);
        //初始化最后一个页目录表项和对应的页表。注意，页表中的页表项不一定足够1024个
        cur_pde_entry->present = 1;
        cur_pde_entry->flags = VM_WRITE;
        cur_pde_entry->globalPage = 1;
        cur_pte = (pte_t *)Alloc_Page();
        KASSERT(cur_pte != NULL);
        memset(cur_pte, 0, PAGE_SIZE);
        cur_pde_entry->pageTableBaseAddr = PAGE_ALLIGNED_ADDR(cur_pte);

        int last_pagetable_num;
        last_pagetable_num = whole_pages % NUM_PAGE_TABLE_ENTRIES;
        //注意当last_pagetable_num=0时，意味着最后一个页目录项对应的页表是满的，就是说页表中1024个页表项都指向一个有效的页。
        if (last_pagetable_num == 0)
        {
            last_pagetable_num = NUM_PAGE_TABLE_ENTRIES;
        }

        for (j = 0; j < last_pagetable_num; j++)
        {
            cur_pte->present = 1;
            cur_pte->flags = VM_WRITE;
            cur_pte->globalPage = 1;
            cur_pte->pageBaseAddr = mem_addr >> 12;
            cur_pte++;
            mem_addr += PAGE_SIZE;
        }
        //从现在开始，系统的寻址必须经过分页机制转换，以前仅仅经过分段机制转换
        Enable_Paging(g_kernel_pde);

        Install_Interrupt_Handler(14, Page_Fault_Handler);
    }
}
/**
 * Initialize paging file data structures.
 * All filesystems should be mounted before this function
 * is called, to ensure that the paging file is available.
 */
void Init_Paging(void)
{
    // TODO("Initialize paging file data structures");
    pagingDevice = Get_Paging_Device();
    if (pagingDevice == NULL)
    {
        Print("can not find pagefile\n");
        return;
    }
    numPagingDiskPages = pagingDevice->numSectors / SECTORS_PER_PAGE;
    //为pagefile中每一页设置标示位
    Bitmap = Create_Bit_Set(numPagingDiskPages);
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
    // TODO("Find free page in paging file");
    return Find_First_Free_Bit(Bitmap, numPagingDiskPages);
}

/**
 * Free a page-sized chunk of disk space in the paging file.
 * Interrupts must be disabled.
 * @param pagefileIndex index of the chunk of disk space
 */
void Free_Space_On_Paging_File(int pagefileIndex)
{
    KASSERT(!Interrupts_Enabled());
    // TODO("Free page in paging file");
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
void Write_To_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex)
{
    struct Page *page = Get_Page((ulong_t)paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); /* Page must be locked! */
    // TODO("Write page data to paging file");
    if (0 <= pagefileIndex && pagefileIndex < numPagingDiskPages)
    {
        int i;
        for (i = 0; i < SECTORS_PER_PAGE; i++)
        {
            Block_Write(
                pagingDevice->dev,
                pagefileIndex * SECTORS_PER_PAGE + i + (pagingDevice->startSector),
                paddr + i * SECTOR_SIZE);
        }
        Set_Bit(Bitmap, pagefileIndex);
    }
    else
    {
        Print("Write_To_Paging_File: pagefileIndex out of range!\n");
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
void Read_From_Paging_File(void *paddr, ulong_t vaddr, int pagefileIndex)
{
    block_t *block = (block_t *)paddr;
    struct Page *page = Get_Page((ulong_t)paddr);
    KASSERT(!(page->flags & PAGE_PAGEABLE)); // Page must be locked!
    // TODO("Read page data from paging file");
    page->flags = page->flags & ~PAGE_PAGEABLE;
}
