/*
 * Paging-based user mode implementation
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.50 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/int.h>
#include <geekos/mem.h>
#include <geekos/paging.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/argblock.h>
#include <geekos/kthread.h>
#include <geekos/range.h>
#include <geekos/vfs.h>
#include <geekos/user.h>
#include <geekos/gdt.h>
#include <geekos/argblock.h>

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */
/**
 * 展示内存
 */
void DisplayMemory(pde_t *pde)
{
    int i;
    char binary[32];
    pte_t *pte = 0;
    Set_Current_Attr(ATTRIB(BLACK, AMBER | BRIGHT));
    Print("Page Directory\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));
    Print("%10s\t%10s\n", "pde", "value");
    Print("%10x\t%10x\n",
          &pde[PAGE_DIRECTORY_INDEX(USER_VM_END)],
          pde[PAGE_DIRECTORY_INDEX(USER_VM_END)]);
    Set_Current_Attr(ATTRIB(BLACK, AMBER | BRIGHT));
    Print("Page Table\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));
    Print("%4s\t%10s\t%10s\t%10s\t%10s\n", "idx", "pte addr", "pte value", "pf addr", "pf value");
    pte = pde[PAGE_DIRECTORY_INDEX(USER_VM_END)].pageTableBaseAddr << 12;
    for (i = 1022; i < 1024; i++)
    {
        Print("%10d\t%10x\t%10x\t%10x\t%10x\n",
              i,
              &pte[i],
              pte[i],
              pte[i].pageBaseAddr << 12,
              *(int *)(pte[i].pageBaseAddr << 12));
    }
}
/** 
 * 创建一个在分页模式下使用的User_Context
 */
struct User_Context *Create_User_Context()
{
    struct User_Context *user_context;
    user_context = (struct User_Context *)Malloc(sizeof(struct User_Context));
    if (user_context == NULL)
    {
        Print("malloc User_Context fail in Create_User_Context/n");
        return NULL;
    }
    user_context->ldtDescriptor = NULL;
    user_context->memory = NULL;
    user_context->size = 0;
    user_context->ldtSelector = 0;
    user_context->csSelector = 0;
    user_context->dsSelector = 0;

    user_context->pageDir = NULL;
    user_context->entryAddr = 0;
    user_context->argBlockAddr = 0;
    user_context->stackPointerAddr = 0;
    user_context->refCount = 0;
    return user_context;
}
/**
 * 把线性地址转换为物理地址
 * 成功返回对应物理地址，失败返回NULL
 */
uint_t Lin_To_Phyaddr(pde_t *page_dir, uint_t lin_address)
{
    uint_t pagedir_index = PAGE_DIRECTORY_INDEX(lin_address);
    uint_t page_index = PAGE_IDNEX(lin_address);
    uint_t offset_address = lin_address & 0xfff;

    pde_t *pagedir_entry = page_dir + pagedir_index;
    pte_t *page_entry = 0;

    if (pagedir_entry->present)
    {
        page_entry = (pte_t *)PAGE_ADDR(pagedir_entry->pageTableBaseAddr);
        page_entry += page_index;
        if (page_entry->present == 0)
        {
            Print("the page do not present!\n");
            KASSERT(0);
        }
        return PAGE_ADDR(page_entry->pageBaseAddr) + offset_address;
    }
    else
    {
        Print("Trying to resolve non-existent address%u/n", lin_address);
        return 0;
    }
}
/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */
/*
 * 将存储在缓冲区中的段信息读入到线性地址对应的页中
 * 成功返回true,失败返回false
 */
bool Copy_User_Page(pde_t *page_dir, uint_t dest_user, char *src, uint_t byte_num)
{
    uint_t phy_start;
    uint_t temp_len;
    int page_nums;
    struct Page *page;

    //检测进程所需用户内存空间占用多少页面。page_nums==1,占用一页;page_nums==0,占用一页以上
    if (Round_Down_To_Page(dest_user + byte_num) == Round_Down_To_Page(dest_user))
    {
        temp_len = byte_num;
        page_nums = 1;
    }
    else
    {
        temp_len = Round_Up_To_Page(dest_user) - dest_user;
        byte_num -= temp_len;
        page_nums = 0;
    }

    phy_start = Lin_To_Phyaddr(page_dir, dest_user);
    if (phy_start == 0)
    {
        return false;
    }
    page = Get_Page(phy_start);
    //保证在复制过程中，所用的的页不会因缺页中断被调换出去
    Disable_Interrupts();
    page->flags &= ~PAGE_PAGEABLE;
    Enable_Interrupts();
    //复制第一页内容
    memcpy((char *)phy_start, src, temp_len);
    page->flags |= PAGE_PAGEABLE;

    if (page_nums == 1)
    {
        return true;
    }

    //对于占用一页以上的情况，处理中间部分的页（此部分的所占用的页数可能为0）
    temp_len = Round_Up_To_Page(dest_user) - dest_user;
    dest_user += temp_len;
    src += temp_len;
    byte_num -= temp_len;

    while (dest_user != Round_Down_To_Page(dest_user + byte_num))
    {

        phy_start = Lin_To_Phyaddr(page_dir, dest_user);
        if (phy_start == 0)
        {
            return false;
        }
        page = Get_Page(phy_start);
        //保证在复制过程中，所用的的页不会因缺页中断被调换出去
        Disable_Interrupts();
        page->flags &= ~PAGE_PAGEABLE;
        Enable_Interrupts();

        memcpy((char *)phy_start, src, PAGE_SIZE);
        page->flags |= PAGE_PAGEABLE;

        dest_user += PAGE_SIZE;
        byte_num -= PAGE_SIZE;
        src += PAGE_SIZE;
    }

    //处理最后一页
    phy_start = Lin_To_Phyaddr(page_dir, dest_user);
    if (phy_start == 0)
    {
        return false;
    }
    //保证在复制过程中，所用的的页不会因缺页中断被调换出去
    Disable_Interrupts();
    page->flags &= ~PAGE_PAGEABLE;
    Enable_Interrupts();

    memcpy((char *)phy_start, src, byte_num);
    page->flags |= PAGE_PAGEABLE;
    return true;
}

/*
 * 分配文件所需要的页
 * 失败返回false，成功返回true
 */
bool Alloc_User_Page(pde_t *pageDir, uint_t startAddress, uint_t sizeInMemory)
{
    uint_t pagedir_index = PAGE_DIRECTORY_INDEX(startAddress);
    uint_t page_index = PAGE_IDNEX(startAddress);

    pde_t *pagedir_entry = pageDir + pagedir_index;
    pte_t *page_entry;

    //建立startAddress对应的页目录表项与页表
    // Print("pagedir_entry=%x\n", pagedir_entry);

    //startAddress对应的页目录表项已经建立
    if (pagedir_entry->present)
    {
        page_entry = (pte_t *)PAGE_ADDR(pagedir_entry->pageTableBaseAddr);
        Print("existed page_entry first=%x\n", page_entry);
    }
    else
    {
        //应页目录表项没有建立
        //分配一个页表
        page_entry = (pte_t *)Alloc_Page();
        if (page_entry == NULL)
        {
            Print("can not allocate page in Alloc_User_Page\n");
            return false;
        }
        memset(page_entry, 0, PAGE_SIZE);
        //设置对应的页目录表项
        *((uint_t *)pagedir_entry) = 0;
        pagedir_entry->present = 1;
        pagedir_entry->flags = VM_WRITE | VM_READ | VM_USER;
        pagedir_entry->globalPage = 0;
        pagedir_entry->pageTableBaseAddr = PAGE_TABLE_INDEX(page_entry);

        Print("new page_entry first=%x\n", page_entry);
    }

    //找到页表中对应于startAddress的页表项
    page_entry += page_index;
    Print("page_entry=%x\n", page_entry);

    //建立startAddress对应的页表项与页
    int num_pages;
    void *page_addr;
    num_pages = Round_Up_To_Page(startAddress - Round_Down_To_Page(startAddress) + sizeInMemory) / PAGE_SIZE;

    int i;
    uint_t first_page_addr = 0;
    for (i = 0; i < num_pages; i++)
    {
        //对应的页表项没有建立的情况（此时意味着对应的页没有建立）
        if (!page_entry->present)
        {
            page_addr = Alloc_Pageable_Page(page_entry, Round_Down_To_Page(startAddress));
            if (page_addr == NULL)
            {
                Print("can not allocate page in Alloc_User_Page/n");
                return false;
            }
            //设置页表项
            *((uint_t *)page_entry) = 0;
            page_entry->present = 1;
            page_entry->flags = VM_WRITE | VM_READ | VM_USER;
            page_entry->globalPage = 0;
            page_entry->pageBaseAddr = PAGE_ALLIGNED_ADDR(page_addr);
            KASSERT(page_addr != 0);
            if (i == 0)
            {
                first_page_addr = (uint_t)page_addr;
            }
            Print("physical  addr=%x\n", page_addr);
            Print("liner addr=%x\n", startAddress);
        }
        page_entry++;
        startAddress += PAGE_SIZE;
    }
    return true;
}
/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context *context)
{
    /*
     * Hints:
     * - Free all pages, page tables, and page directory for
     *   the process (interrupts must be disabled while you do this,
     *   otherwise those pages could be stolen by other processes)
     * - Free semaphores, files, and other resources used
     *   by the process
     */
    // TODO("Destroy User_Context data structure after process exits");
    if (context == NULL)
    {
        return;
    }

    Free_Segment_Descriptor(context->ldtDescriptor);
    Set_PDBR(g_kernel_pde);
    if (context->pageDir != NULL)
    {
        pde_t *page_dir = context->pageDir;
        //KASSERT(!Interrupts_Enabled());
        //Enable_Interrupts();
        bool flag;
        flag = Begin_Int_Atomic();

        pde_t *pdir;
        if (page_dir == NULL)
        {
            return true;
        }
        for (pdir = page_dir + NUM_PAGE_DIR_ENTRIES / 2; pdir < page_dir + NUM_PAGE_DIR_ENTRIES; pdir++)
        {
            pte_t *ptable;
            pte_t *ptable_first;
            if (!pdir->present)
            {
                continue;
            }
            ptable_first = (pte_t *)(pdir->pageTableBaseAddr << 12);
            for (ptable = ptable_first; ptable < ptable_first + NUM_PAGE_TABLE_ENTRIES; ptable++)
            {
                if (ptable->present)
                {
                    Free_Page((void *)(ptable->pageBaseAddr << 12));
                }
                else if (ptable->kernelInfo == KINFO_PAGE_ON_DISK)
                {
                    //当页在pagefile上时，pte_t结构中的pageBaseAddr指示了页在pagefile中的位置
                    Free_Space_On_Paging_File(ptable->pageBaseAddr);
                }
            }
            Free_Page(ptable_first);
        }
        //Disable_Interrupts();
        Free_Page(page_dir);

        End_Int_Atomic(flag);
    }
    context->pageDir = 0;
    Free(context);
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
                      struct Exe_Format *exeFormat, const char *command,
                      struct User_Context **pUserContext)
{
    /*
     * Hints:
     * - This will be similar to the same function in userseg.c
     * - Determine space requirements for code, data, argument block,
     *   and stack
     * - Allocate pages for above, map them into user address
     *   space (allocating page directory and page tables as needed)
     * - Fill in initial stack pointer, argument block address,
     *   and code entry point fields in User_Context
     */
    // TODO("Load user program into address space");
    struct User_Context *uContext;
    uContext = Create_User_Context(); //创建一个UserContext

    //处理分页涉及的数据
    pde_t *pageDirectory;
    pageDirectory = (pde_t *)Alloc_Page();
    if (pageDirectory == NULL)
    {
        Print("no more page!\n");
        return -1;
    }
    memset(pageDirectory, 0, PAGE_SIZE);
    //将内核页目录复制到用户态进程的页目录中
    memcpy(pageDirectory, g_kernel_pde, PAGE_SIZE);
    //将用户态进程对应高2G线性地址的页目录表项置为0，用户态进程中高2G的线性地址在GeekOS中为用户空间
    memset(pageDirectory + 512, 0, PAGE_SIZE / 2);

    uContext->pageDir = pageDirectory;

    int i;
    int res;
    uint_t startAddress = 0;
    uint_t sizeInMemory = 0;
    uint_t offsetInFile = 0;
    uint_t lengthInFile = 0;
    for (i = 0; i < exeFormat->numSegments - 1; i++)
    {
        startAddress = exeFormat->segmentList[i].startAddress;
        sizeInMemory = exeFormat->segmentList[i].sizeInMemory;

        offsetInFile = exeFormat->segmentList[i].offsetInFile;
        lengthInFile = exeFormat->segmentList[i].lengthInFile;
        if (!sizeInMemory && !lengthInFile)
        {
            sizeInMemory = DEFAULT_STACK_SIZE;
            lengthInFile = DEFAULT_STACK_SIZE;
        }

        if (startAddress + sizeInMemory < USER_VM_LEN)
        {
            //给数据段和代码段分配空间
            //在GeekOS的分页机制下，用户地址空间默认从线性地址2G开始
            if (!Alloc_User_Page(pageDirectory, startAddress + USER_VM_START, sizeInMemory) ||
                !Copy_User_Page(pageDirectory, startAddress + USER_VM_START, exeFileData + offsetInFile, lengthInFile))
            {
                return -1;
            }
        }
        else
        {
            Print("startAddress+sizeInMemory > 2GB in Load_User_Program\n");
            return -1;
        }
    }

    //处理参数块
    uint_t args_num;
    uint_t stack_addr;
    uint_t arg_addr;
    ulong_t arg_size;
    Get_Argument_Block_Size(command, &args_num, &arg_size);
    if (arg_size > PAGE_SIZE)
    {
        Print("Argument Block too big for one PAGE_SIZE\n");
        return -1;
    }
    //给参数块在地址空间的尾部分配一页
    arg_addr = Round_Down_To_Page(USER_VM_LEN - arg_size);
    char *block_buffer = Malloc(arg_size);
    KASSERT(block_buffer != NULL);
    Format_Argument_Block(block_buffer, args_num, arg_addr, command);

    if (!Alloc_User_Page(pageDirectory, arg_addr + USER_VM_START, arg_size) ||
        !Copy_User_Page(pageDirectory, arg_addr + USER_VM_START, block_buffer, arg_size))
    {
        return -1;
    }
    Free(block_buffer);

    //给堆栈在地址空间的尾部分配一页
    stack_addr = USER_VM_LEN - Round_Up_To_Page(arg_size) - DEFAULT_STACK_SIZE;
    if (!Alloc_User_Page(pageDirectory, stack_addr + USER_VM_START, DEFAULT_STACK_SIZE))
    {
        return -1;
    }
    //UserContext中涉及分段机制的选择子,描述符
    uContext->ldtDescriptor = Allocate_Segment_Descriptor();
    if (uContext->ldtDescriptor == NULL)
    {
        Print("allocate segment descriptor fail\n");
        return -1;
    }

    Init_LDT_Descriptor(uContext->ldtDescriptor, uContext->ldt, NUM_USER_LDT_ENTRIES);
    uContext->ldtSelector = Selector(USER_PRIVILEGE, true, Get_Descriptor_Index(uContext->ldtDescriptor));

    //注意，在GeekOS的分页机制下，用户地址空间默认从线性地址2G开始
    Init_Code_Segment_Descriptor(&uContext->ldt[0], USER_VM_START, USER_VM_LEN / PAGE_SIZE, USER_PRIVILEGE);
    Init_Data_Segment_Descriptor(&uContext->ldt[1], USER_VM_START, USER_VM_LEN / PAGE_SIZE, USER_PRIVILEGE);

    uContext->csSelector = Selector(USER_PRIVILEGE, false, 0);
    uContext->dsSelector = Selector(USER_PRIVILEGE, false, 1);

    //填充UserContext
    uContext->entryAddr = exeFormat->entryAddr;
    uContext->argBlockAddr = arg_addr;
    uContext->size = USER_VM_LEN;
    uContext->stackPointerAddr = stack_addr;
    *pUserContext = uContext;

    DisplayMemory(pageDirectory);
    return 0;
}

/*
 * Copy data from user buffer into kernel buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_From_User(void *destInKernel, ulong_t srcInUser, ulong_t numBytes)
{
    /*
     * Hints:
     * - Make sure that user page is part of a valid region
     *   of memory
     * - Remember that you need to add 0x80000000 to user addresses
     *   to convert them to kernel addresses, because of how the
     *   user code and data segments are defined
     * - User pages may need to be paged in from disk before being accessed.
     * - Before you touch (read or write) any data in a user
     *   page, **disable the PAGE_PAGEABLE bit**.
     *
     * Be very careful with race conditions in reading a page from disk.
     * Kernel code must always assume that if the struct Page for
     * a page of memory has the PAGE_PAGEABLE bit set,
     * IT CAN BE STOLEN AT ANY TIME.  The only exception is if
     * interrupts are disabled; because no other process can run,
     * the page is guaranteed not to be stolen.
     */
    // TODO("Copy user data to kernel buffer");
    struct User_Context *userContext = g_currentThread->userContext;
    void *user_lin_addr = (void *)(USER_VM_START) + srcInUser;

    if ((srcInUser + numBytes) < userContext->size)
    {
        memcpy(destInKernel, user_lin_addr, numBytes);
        return true;
    }
    return false;
}

/*
 * Copy data from kernel buffer into user buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_To_User(ulong_t destInUser, void *srcInKernel, ulong_t numBytes)
{
    /*
     * Hints:
     * - Same as for Copy_From_User()
     * - Also, make sure the memory is mapped into the user
     *   address space with write permission enabled
     */
    // TODO("Copy kernel data to user buffer");
    struct User_Context *userContext = g_currentThread->userContext;
    void *user_lin_addr = (void *)(USER_VM_START) + destInUser;
    if ((destInUser + numBytes) < userContext->size)
    {
        memcpy(user_lin_addr, srcInKernel, numBytes);
        return true;
    }
    return false;
}

/*
 * Switch to user address space.
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * - If you are still using an LDT to define your user code and data
     *   segments, switch to the process's LDT
     * - 
     */
    // TODO("Switch_To_Address_Space() using paging");
    if (userContext == 0)
    {
        Print("the userContext is NULL!/n");
        return;
    }
    Load_LDTR(userContext->ldtSelector);
    Set_PDBR(userContext->pageDir);
}
