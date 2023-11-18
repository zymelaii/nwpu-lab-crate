/*
 * System call handlers
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.59 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/syscall.h>
#include <geekos/errno.h>
#include <geekos/kthread.h>
#include <geekos/int.h>
#include <geekos/elf.h>
#include <geekos/malloc.h>
#include <geekos/screen.h>
#include <geekos/keyboard.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/timer.h>
#include <geekos/vfs.h>
#include <geekos/synch.h>

/*
 * Null system call.
 * Does nothing except immediately return control back
 * to the interrupted user program.
 * Params:
 *  state - processor registers from user mode
 *
 * Returns:
 *   always returns the value 0 (zero)
 */
static int Sys_Null(struct Interrupt_State *state)
{
    return 0;
}

/*
 * Exit system call.
 * The interrupted user process is terminated.
 * Params:
 *   state->ebx - process exit code
 * Returns:
 *   Never returns to user mode!
 */
static int Sys_Exit(struct Interrupt_State *state)
{
    // TODO("Exit system call");
    Exit(state->ebx);
}

static int Copy_User_String(ulong_t uaddr, ulong_t len,
                            ulong_t maxLen, char **pStr)
{
    int result = 0;
    char *str;
    /* 字符串超过最大长度 */
    if (len > maxLen)
        return EINVALID;
    /* 为字符串分配内存空间 */
    str = (char *)Malloc(len + 1);
    if (str == 0)
    {
        return ENOMEM;
        goto fail;
    }
    /* 从用户内存空间中复制字符串到系统内核空间 */
    if (!Copy_From_User(str, uaddr, len))
    {
        result = EINVALID;
        Free(str);
        goto fail;
    }
    str[len] = '\0';
    /* 拷贝成功 */
    *pStr = str;
fail:
    return result;
}

/*
 * Print a string to the console.
 * Params:
 *   state->ebx - user pointer of string to be printed
 *   state->ecx - number of characters to print
 * Returns: 0 if successful, -1 if not
 */
static int Sys_PrintString(struct Interrupt_State *state)
{
    // TODO("PrintString system call");
    int result = 0;
    uint_t length = state->ecx; //字符串长度
    uchar_t *buf = 0;
    if (length > 0)
    {
        /* 将字符串复制到系统内核空间 */
        if (Copy_User_String(state->ebx, length, 1023, (char **)&buf) != 0)
            goto done;
        /* 输出字符串到控制台 */
        Put_Buf(buf, length);
    }
done:
    if (buf != NULL)
        Free(buf);
    return result;
}

/*
 * Get a single key press from the console.
 * Suspends the user process until a key press is available.
 * Params:
 *   state - processor registers from user mode
 * Returns: the key code
 */
static int Sys_GetKey(struct Interrupt_State *state)
{
    // TODO("GetKey system call");
    return Wait_For_Key();
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State *state)
{
    // TODO("SetAttr system call");
    Set_Current_Attr((uchar_t)state->ebx);
    return 0;
}

/*
 * Get the current cursor position.
 * Params:
 *   state->ebx - pointer to user int where row value should be stored
 *   state->ecx - pointer to user int where column value should be stored
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_GetCursor(struct Interrupt_State *state)
{
    // TODO("GetCursor system call");
    /* 获取当前光标所在屏幕位置(行和列) */
    int row, col;
    Get_Cursor(&row, &col);
    if (!Copy_To_User(state->ebx, &row, sizeof(int)) ||
        !Copy_To_User(state->ecx, &col, sizeof(int)))
        return -1;
    return 0;
}

/*
 * Set the current cursor position.
 * Params:
 *   state->ebx - new row value
 *   state->ecx - new column value
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_PutCursor(struct Interrupt_State *state)
{
    // TODO("PutCursor system call");
    return Put_Cursor(state->ebx, state->ecx) ? 0 : -1;
}

/*
 * Create a new user process.
 * Params:
 *   state->ebx - user address of name of executable
 *   state->ecx - length of executable name
 *   state->edx - user address of command string
 *   state->esi - length of command string
 * Returns: pid of process if successful, error code (< 0) otherwise
 */
static int Sys_Spawn(struct Interrupt_State *state)
{
    // TODO("Spawn system call");
    int res;           //程序返回值
    char *program = 0; //进程名称
    char *command = 0; //用户命令
    struct Kernel_Thread *process;
    /* 复制程序名和命令字符串到用户内存空间 */
    res = Copy_User_String(state->ebx, state->ecx, VFS_MAX_PATH_LEN, &program);
    if (res != 0)
    { //从用户空间复制进程名称
        goto fail;
    }
    res = Copy_User_String(state->edx, state->esi, 1023, &command);
    if (res != 0)
    { //从用户空间复制用户命令
        goto fail;
    }
    /* 生成用户进程 */
    Enable_Interrupts();                     //开中断
    res = Spawn(program, command, &process); //得到进程名称和用户命令后便可生成一个新进程
    if (res == 0)
    { //若成功则返回新进程ID号
        KASSERT(process != 0);
        res = process->pid;
    }
    Disable_Interrupts(); //关中断
fail:
    if (program != 0)
        Free(program);
    if (command != 0)
        Free(command);
    return res;
}
/*
 * Wait for a process to exit.
 * Params:
 *   state->ebx - pid of process to wait for
 * Returns: the exit code of the process,
 *   or error code (< 0) on error
 */
static int Sys_Wait(struct Interrupt_State *state)
{
    // TODO("Wait system call");
    int exitCode;
    /* 查找需要等待的进程 */
    struct Kernel_Thread *kthread = Lookup_Thread(state->ebx);
    /* 如果没有找到需要等待的进程，则返回错误代码 */
    if (kthread == 0)
        return -1;
    /* 等待指定进程结束 */
    Enable_Interrupts();
    exitCode = Join(kthread);
    Disable_Interrupts();
    return exitCode;
}

/*
 * Get pid (process id) of current thread.
 * Params:
 *   state - processor registers from user mode
 * Returns: the pid of the current thread
 */
static int Sys_GetPID(struct Interrupt_State *state)
{
    // TODO("GetPID system call");
    return g_currentThread->pid;
}

/**
 * 设置调度策略
 * Set the scheduling policy.
 * Params:
 *   state->ebx - policy,
 *   state->ecx - number of ticks in quantum
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_SetSchedulingPolicy(struct Interrupt_State *state)
{
    // TODO("SetSchedulingPolicy system call");
    /* 如果输入的优先级调度方法参数无效(非 0 或 1)则返回错误 */
    if (state->ebx != ROUND_ROBIN && state->ebx != MULTILEVEL_FEEDBACK)
    {
        Print("Error! Scheduling Policy should be RR or MLF\n");
        return -1;
    }
    /* 如果输入的时间片参数不在[1, 100]之间则返回错误 */
    if (state->ecx < 1 || state->ecx > 100)
    {
        Print("Error! Quantum should be in the range of [1, 100]\n");
        return -1;
    }

    int res = Chang_Scheduling_Policy(state->ebx, state->ecx);

    return res;
}

/*
 * Get the time of day.
 * Params:
 *   state - processor registers from user mode
 *
 * Returns: value of the g_numTicks global variable
 */
static int Sys_GetTimeOfDay(struct Interrupt_State *state)
{
    // TODO("GetTimeOfDay system call");
    return g_numTicks;
}

/*
 * Create a semaphore.
 * Params:
 *   state->ebx - user address of name of semaphore
 *   state->ecx - length of semaphore name
 *   state->edx - initial semaphore count
 * Returns: the global semaphore id
 */
static int Sys_CreateSemaphore(struct Interrupt_State *state)
{
    // TODO("CreateSemaphore system call");
    int res;
    ulong_t userAddr = state->ebx;  //信号量名字符串所在用户空间地址
    ulong_t nameLen = state->ecx;   //信号量名长度
    ulong_t initCount = state->edx; //信号量初始值
    /* 如果传入参数不正确则返回错误 */
    if (nameLen <= 0 || initCount < 0 || nameLen > MAX_SEMAPHORE_NAME)
    {
        Print("Error! Semaphore Params incorrect\n");
        return EINVALID;
    }

    char *semName = NULL;
    /* 从用户空间拷贝信号量名字符串到内核空间 */
    res = Copy_User_String(userAddr, nameLen, MAX_SEMAPHORE_NAME, &semName);
    if (res != 0)
    {
        Print("Error! Cannot copy string from user spcce\n");
        return res;
    }
    /* 判断信号量名的合法性(中间是否含有'\0'字符) */
    if (strnlen(semName, MAX_SEMAPHORE_NAME) != nameLen)
    {
        Print("Error! Semaphore Name is Invalid\n");
        return EINVALID;
    }
    /* 创建一个信号量 */
    res = Create_Semaphore(semName, nameLen, initCount);

    return res;
}

/*
 * Acquire a semaphore.
 * Assume that the process has permission to access the semaphore,
 * the call will block until the semaphore count is >= 0.
 * Params:
 *   state->ebx - the semaphore id
 *
 * Returns: 0 if successful, error code (< 0) if unsuccessful
 */
static int Sys_P(struct Interrupt_State *state)
{
    // TODO("P (semaphore acquire) system call");
    int sid = state->ebx;
    if (sid <= 0)
    {
        Print("Error! Semaphore ID is Invalid\n");
        return EINVALID;
    }
    return P(sid);
}

/*
 * Release a semaphore.
 * Params:
 *   state->ebx - the semaphore id
 *
 * Returns: 0 if successful, error code (< 0) if unsuccessful
 */
static int Sys_V(struct Interrupt_State *state)
{
    // TODO("V (semaphore release) system call");
    int sid = state->ebx;
    if (sid <= 0)
    {
        Print("Error! Semaphore ID is Invalid\n");
        return EINVALID;
    }
    return V(sid);
}

/*
 * Destroy a semaphore.
 * Params:
 *   state->ebx - the semaphore id
 *
 * Returns: 0 if successful, error code (< 0) if unsuccessful
 */
static int Sys_DestroySemaphore(struct Interrupt_State *state)
{
    // TODO("DestroySemaphore system call");
    int sid = state->ebx;
    if (sid <= 0)
    {
        Print("Error! Semaphore ID is Invalid\n");
        return EINVALID;
    }
    return Destroy_Semaphore(state->ebx);
}

/*
 * Global table of system call handler functions.
 */
const Syscall g_syscallTable[] = {
    Sys_Null,
    Sys_Exit,
    Sys_PrintString,
    Sys_GetKey,
    Sys_SetAttr,
    Sys_GetCursor,
    Sys_PutCursor,
    Sys_Spawn,
    Sys_Wait,
    Sys_GetPID,
    /* Scheduling and semaphore system calls. */
    Sys_SetSchedulingPolicy,
    Sys_GetTimeOfDay,
    Sys_CreateSemaphore,
    Sys_P,
    Sys_V,
    Sys_DestroySemaphore,
};

/*
 * Number of system calls implemented.
 */
const int g_numSyscalls = sizeof(g_syscallTable) / sizeof(Syscall);
