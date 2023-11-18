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

/* ----------------------------------------------------------------------
 * Global variables
 * ---------------------------------------------------------------------- */

int userDebug = 0;
extern pde_t *g_kernel_pde;

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

static struct User_Context* Create_User_Context()
{
    struct User_Context *context = (struct User_Context*)Malloc(sizeof(struct User_Context));
    if (context == NULL) { return NULL; }
    memset(context, 0, sizeof(struct User_Context));
    return context;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* context)
{
    /*
     * Hints:
     * - Free all pages, page tables, and page directory for
     *   the process (interrupts must be disabled while you do this,
     *   otherwise those pages could be stolen by other processes)
     * - Free semaphores, files, and other resources used
     *   by the process
     */
    KASSERT(context->refCount == 0);
    Free_Segment_Descriptor(context->ldtDescriptor);
    bool flag = Begin_Int_Atomic();
    Free_User_Pages(context);
    Free(context);
    End_Int_Atomic(flag);
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

    struct User_Context *uContext = Create_User_Context();
    uContext->ldtDescriptor = Allocate_Segment_Descriptor();
    if (uContext->ldtDescriptor == NULL) {
        Print("allocate segment descriptor fail\n");
        return -1;
    }
    Init_LDT_Descriptor(uContext->ldtDescriptor, uContext->ldt, NUM_USER_LDT_ENTRIES);
    uContext->ldtSelector = Selector(USER_PRIVILEGE, true, Get_Descriptor_Index(uContext->ldtDescriptor));
    Init_Code_Segment_Descriptor(&uContext->ldt[0], USER_VM_START, USER_VM_LEN / PAGE_SIZE, USER_PRIVILEGE);
    Init_Data_Segment_Descriptor(&uContext->ldt[0], USER_VM_START, USER_VM_LEN / PAGE_SIZE, USER_PRIVILEGE);
    uContext->csSelector = Selector(USER_PRIVILEGE, false, 0);
    uContext->dsSelector = Selector(USER_PRIVILEGE, false, 1);

    pde_t *pageDir = (pde_t*)Alloc_Page();
    if (pageDir == NULL) { return -1; }
    memset(pageDir, 0, PAGE_SIZE);
    memcpy(pageDir, g_kernel_pde, PAGE_SIZE);
    uContext->pageDir = pageDir;
    uint_t startAddress = 0;
    uint_t sizeInMemory = 0;
    uint_t offsetInFile = 0;
    uint_t lengthInFile = 0;
    for (int i = 0; i < exeFormat->numSegments; ++i) {
        startAddress = exeFormat->segmentList[i].startAddress;
        sizeInMemory = exeFormat->segmentList[i].sizeInMemory;
        offsetInFile = exeFormat->segmentList[i].offsetInFile;
        lengthInFile = exeFormat->segmentList[i].lengthInFile;
        if (startAddress + sizeInMemory < USER_VM_LEN) {
            int res = Alloc_User_Page(pageDir, startAddress + USER_VM_START, sizeInMemory);
            if (res != 0) { return -1; }
            if (sizeInMemory == 0 && lengthInFile == 0) { continue; }
            res = Copy_User_Page(pageDir, startAddress + USER_VM_START, exeFileData + offsetInFile, lengthInFile);
            if (!res) { return -1; }
        } else {
            return -1;
        }
    }

    uint_t args_num, stack_addr, arg_addr;
    ulong_t arg_size;
    Get_Argument_Block_Size(command, &args_num, &arg_size);
    if (arg_size > PAGE_SIZE) { return -1; }
    arg_addr = Round_Down_To_Page(USER_VM_LEN - arg_size);
    char *block_buffer = Malloc(arg_size);
    KASSERT(block_buffer != NULL);
    Format_Argument_Block(block_buffer, args_num, arg_addr, command);
    int res = Alloc_User_Page(pageDir, arg_addr + USER_VM_START, arg_size);
    if (res != 0) { return -1; }
    res = Copy_User_Page(pageDir, arg_addr + USER_VM_START, block_buffer, arg_size);
    if (!res) { return -1; }
    Free(block_buffer);

    stack_addr = USER_VM_LEN - Round_Down_To_Page(arg_size) - DEFAULT_STACK_SIZE;
    res = Alloc_User_Page(pageDir, stack_addr + USER_VM_START, DEFAULT_STACK_SIZE);
    if (res != 0) { return -1; }

    uContext->entryAddr = exeFormat->entryAddr;
    uContext->argBlockAddr = arg_addr;
    uContext->size = USER_VM_LEN;
    uContext->stackPointerAddr = arg_addr;
    *pUserContext = uContext;
    return 0;
}

/*
 * Copy data from user buffer into kernel buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t numBytes)
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

    void *kaddr = destInKernel;
    ulong_t user_va = srcInUser + USER_VM_START;
    ulong_t copied = 0;
    struct User_Context *context = g_currentThread->userContext;
    while (copied < numBytes) {
        struct Page *cur_page = Get_Page(lin_to_phyaddr(context->pageDir, user_va));
        bool flag = Begin_Int_Atomic();
        cur_page->flags &= ~PAGE_PAGEABLE;
        End_Int_Atomic(flag);
        ulong_t to_copy = PAGE_SIZE;
        if (!Is_Page_Multiple(user_va)) {
            to_copy -= user_va & (PAGE_SIZE - 1);
        }
        if (to_copy > numBytes) {
            to_copy = numBytes;
        }
        memcpy(kaddr, (void*)user_va, to_copy);
        user_va = Round_Down_To_Page(user_va + PAGE_SIZE);
        kaddr = (void*)((char*)kaddr + to_copy);
        copied += to_copy;
        flag = Begin_Int_Atomic();
        cur_page->flags |= PAGE_PAGEABLE;
        End_Int_Atomic(flag);
    }
    return true;
}

/*
 * Copy data from kernel buffer into user buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t numBytes)
{
    /*
     * Hints:
     * - Same as for Copy_From_User()
     * - Also, make sure the memory is mapped into the user
     *   address space with write permission enabled
     */

    ulong_t user_va = destInUser + USER_VM_START;
    ulong_t copied = 0;
    struct User_Context *context = g_currentThread->userContext;
    while (copied < numBytes) {
        struct Page *cur_page = Get_Page(lin_to_phyaddr(context->pageDir, user_va));
        cur_page->flags &= ~PAGE_PAGEABLE;
        ulong_t to_copy = PAGE_SIZE;
        if (!Is_Page_Multiple(user_va)) {
            to_copy -= user_va & (PAGE_SIZE - 1);
        }
        if (to_copy > numBytes) {
            to_copy = numBytes;
        }
        memcpy((void*)user_va, srcInKernel, to_copy);
        user_va = Round_Down_To_Page(user_va + PAGE_SIZE);
        srcInKernel = (void*)((char*)srcInKernel + to_copy);
        copied += to_copy;
        cur_page->flags |= PAGE_PAGEABLE;
    }
    return true;
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
    Set_PDBR(userContext->pageDir);
    ushort_t ldtSelector = userContext->ldtSelector;
    asm volatile ("lldt %0"::"a"(ldtSelector));
}
