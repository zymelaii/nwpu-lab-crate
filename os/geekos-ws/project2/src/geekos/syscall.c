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

static int Copy_User_String(ulong_t uaddr, ulong_t len, ulong_t maxLen, char **pStr) {
    int rc = 0;
    char *str;
    if (len > maxLen) { return EINVALID; }
    str = (char*)Malloc(len + 1);
    if (str == NULL) {
        rc = ENOMEM;
        goto done;
    }
    if (!Copy_From_User(str, uaddr, len)) {
        rc = EINVALID;
        Free(str);
        goto done;
    }
    str[len] = '\0';
    *pStr = str;
done:
    return rc;
}

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
static int Sys_Null(struct Interrupt_State* state)
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
static int Sys_Exit(struct Interrupt_State* state)
{
    Exit(state->ebx);
}

/*
 * Print a string to the console.
 * Params:
 *   state->ebx - user pointer of string to be printed
 *   state->ecx - number of characters to print
 * Returns: 0 if successful, -1 if not
 */
static int Sys_PrintString(struct Interrupt_State* state)
{
    int rc = 0;
    uint_t len = state->ecx;
    char *buf = 0;
    if (len > 0) {
        if ((rc = Copy_User_String(state->ebx, len, 1023, (char**)&buf)) != 0) {
            goto done;
        }
        Put_Buf(buf, len);
    }
done:
    if (buf != 0) {
        Free(buf);
    }
    return rc;
}

/*
 * Get a single key press from the console.
 * Suspends the user process until a key press is available.
 * Params:
 *   state - processor registers from user mode
 * Returns: the key code
 */
static int Sys_GetKey(struct Interrupt_State* state)
{
    return Wait_For_Key();
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State* state)
{
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
static int Sys_GetCursor(struct Interrupt_State* state)
{
    int row, col;
    Get_Cursor(&row, &col);
    if (!Copy_To_User(state->ebx, &row, sizeof(int)) || !Copy_To_User(state->ecx, &col, sizeof(int))) {
        return -1;
    }
    return 0;
}

/*
 * Set the current cursor position.
 * Params:
 *   state->ebx - new row value
 *   state->ecx - new column value
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_PutCursor(struct Interrupt_State* state)
{
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
static int Sys_Spawn(struct Interrupt_State* state)
{
    int rc;
    char *program = 0;
    char *command = 0;
    struct Kernel_Thread *process;
    if ((rc = Copy_User_String(state->ebx, state->ecx, VFS_MAX_PATH_LEN, &program)) != 0
        || (rc = Copy_User_String(state->edx, state->esi, 1023, &command)) != 0) {
            goto done;
        }
    Enable_Interrupts();
    rc = Spawn(program, command, &process);
    if (rc == 0) {
        KASSERT(process != 0);
        rc = process->pid;
    }
    Disable_Interrupts();
done:
    if (program != 0) {
        Free(program);
    }
    if (command != 0) {
        Free(command);
    }
    return rc;
}

/*
 * Wait for a process to exit.
 * Params:
 *   state->ebx - pid of process to wait for
 * Returns: the exit code of the process,
 *   or error code (< 0) on error
 */
static int Sys_Wait(struct Interrupt_State* state)
{
    int exitCode;
    struct Kernel_Thread *kthread = Lookup_Thread(state->ebx);
    if (kthread == 0) {
        return -1;
    }
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
static int Sys_GetPID(struct Interrupt_State* state)
{
    return g_currentThread->pid;
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
};

/*
 * Number of system calls implemented.
 */
const int g_numSyscalls = sizeof(g_syscallTable) / sizeof(Syscall);
