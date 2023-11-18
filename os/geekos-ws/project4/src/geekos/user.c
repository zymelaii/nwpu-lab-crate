/*
 * Common user mode functions
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.50 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/int.h>
#include <geekos/mem.h>
#include <geekos/malloc.h>
#include <geekos/kthread.h>
#include <geekos/vfs.h>
#include <geekos/tss.h>
#include <geekos/user.h>

/*
 * This module contains common functions for implementation of user
 * mode processes.
 */

/*
 * Associate the given user context with a kernel thread.
 * This makes the thread a user process.
 */
void Attach_User_Context(struct Kernel_Thread *kthread, struct User_Context *context)
{
    KASSERT(context != 0);
    kthread->userContext = context;

    Disable_Interrupts();

    /*
     * We don't actually allow multiple threads
     * to share a user context (yet)
     */
    KASSERT(context->refCount == 0);

    ++context->refCount;
    Enable_Interrupts();
}

/*
 * If the given thread has a user context, detach it
 * and destroy it.  This is called when a thread is
 * being destroyed.
 */
void Detach_User_Context(struct Kernel_Thread *kthread)
{
    struct User_Context *old = kthread->userContext;

    kthread->userContext = 0;

    if (old != 0)
    {
        int refCount;

        Disable_Interrupts();
        --old->refCount;
        refCount = old->refCount;
        Enable_Interrupts();

        /*Print("User context refcount == %d\n", refCount);*/
        if (refCount == 0)
            Destroy_User_Context(old);
    }
}

/**
 * Spawn a user process.
 * Params:
 * @param program 要读入内存缓冲区的可执行文件 - the full path of the program executable file
 * @param command 用户执行程序执行时的命令行字符串 - the command, including name of program and arguments
 * @param pThread 存放指向刚创建进程的指针 - reference to Kernel_Thread pointer where a pointer to
 *     the newly created user mode thread (process) should be
 *     stored
 * @return 
 *   The process id (pid) of the new process, or an error code
 *   if the process couldn't be created.  Note that this function
 *   should return ENOTFOUND if the reason for failure is that
 *   the executable file doesn't exist.
 */
int Spawn(const char *program, const char *command, struct Kernel_Thread **pThread)
{
    /*
     * Hints:
     * - Call Read_Fully() to load the entire executable into a memory buffer
     * - Call Parse_ELF_Executable() to verify that the executable is
     *   valid, and to populate an Exe_Format data structure describing
     *   how the executable should be loaded
     * - Call Load_User_Program() to create a User_Context with the loaded
     *   program
     * - Call Start_User_Thread() with the new User_Context
     *
     * If all goes well, store the pointer to the new thread in
     * pThread and return 0.  Otherwise, return an error code.
     */
    // TODO("Spawn a process by reading an executable from a filesystem");
    int res;

    /* 读取 ELF 文件 */
    char *exeFileData = NULL;
    ulong_t exeFileLength = 0;
    res = Read_Fully(program, (void **)&exeFileData, &exeFileLength);
    if (res != 0)
    {
        if (exeFileData != NULL)
            Free(exeFileData);
        return ENOTFOUND;
    }

    /* 分析 ELF 文件 */
    struct Exe_Format exeFormat;
    res = Parse_ELF_Executable(exeFileData, exeFileLength, &exeFormat);
    if (res != 0)
    {
        if (exeFileData != NULL)
            Free(exeFileData);
        return res;
    }

    /* 加载用户程序 */
    struct User_Context *userContext = NULL;
    res = Load_User_Program(exeFileData, exeFileLength, &exeFormat, command, &userContext);
    if (res != 0)
    {
        if (exeFileData != NULL)
            Free(exeFileData);
        if (userContext != NULL)
            Destroy_User_Context(userContext);
        return res;
    }
    if (exeFileData != NULL)
        Free(exeFileData);
    exeFileData = NULL;

    /* 开始用户进程 */
    struct Kernel_Thread *thread = NULL;
    thread = Start_User_Thread(userContext, false);
    /* 超出内存 创建新进程失败 */
    if (thread == NULL)
    {
        if (userContext != NULL)
            Destroy_User_Context(userContext);
        return ENOMEM;
    }

    KASSERT(thread->refCount == 2);
    /* 返回核心进程的指针 */
    *pThread = thread;
    return (*pThread)->pid;
}

/**
 * If the given thread has a User_Context,
 * switch to its memory space.
 *
 * @param kthread 存放指向进程的指针- the thread that is about to execute
 * @param state - saved processor registers describing the state when
 *      the thread was interrupted
 */
void Switch_To_User_Context(struct Kernel_Thread *kthread, struct Interrupt_State *state)
{
    /*
     * Hint: Before executing in user mode, you will need to call
     * the Set_Kernel_Stack_Pointer() and Switch_To_Address_Space()
     * functions.
     */
    // TODO("Switch to a new user address space, if necessary");

    static struct User_Context *s_currentUserContext;

    //指向User_Conetxt的指针，并初始化为准备切换的进程
    struct User_Context *userContext = kthread->userContext;

    KASSERT(!Interrupts_Enabled());

    //userContext为0表示此进程为核心态进程就不用切换地址空间
    if (userContext == 0)
        return;

    if (userContext != s_currentUserContext)
    {
        //为用户态进程时则切换地址空间
        Switch_To_Address_Space(userContext);
        //新进程的核心栈指针
        ulong_t esp0 = ((ulong_t)kthread->stackPage) + PAGE_SIZE;
        //设置内核堆栈指针
        Set_Kernel_Stack_Pointer(esp0);
        //保存新的 userContxt
        s_currentUserContext = userContext;
    }
}