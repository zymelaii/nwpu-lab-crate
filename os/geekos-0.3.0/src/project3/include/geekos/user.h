/*
 * User mode context
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * $Revision: 1.42 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_USER_H
#define GEEKOS_USER_H

#include <geekos/ktypes.h>
#include <geekos/segment.h>
#include <geekos/elf.h>

struct File;

/* Number of files user process can have open. */
#define USER_MAX_FILES		10

/*
 * A user mode context which can be attached to a Kernel_Thread,
 * to allow it to execute in user mode (ring 3).  This struct
 * has all information needed to create and manage a user
 * memory space, as well as other kernel resources used by
 * the process (such as semaphores and files).
 */
struct User_Context {
    /* We need one LDT entry each for user code and data segments. */
#define NUM_USER_LDT_ENTRIES 2

    /*
     * Each user context contains a local descriptor table with
     * just enough room for one code and one data segment
     * describing the process's memory.
     */
    struct Segment_Descriptor ldt[NUM_USER_LDT_ENTRIES];
    struct Segment_Descriptor* ldtDescriptor;

    /* The memory space used by the process. */
    char* memory;
    ulong_t size;

    /* Selector for the LDT's descriptor in the GDT */
    ushort_t ldtSelector;

    /*
     * Selectors for the user context's code and data segments
     * (which reside in its LDT)
     */
    ushort_t csSelector;
    ushort_t dsSelector;

    /* Code entry point */
    ulong_t entryAddr;

    /* Address of argument block in user memory */
    ulong_t argBlockAddr;

    /* Initial stack pointer */
    ulong_t stackPointerAddr;

    /*
     * May use this in future to allow multiple threads
     * in the same user context
     */
    int refCount;

#if 0
    int *semaphores;
#endif
};

struct Kernel_Thread;
struct Interrupt_State;

/*
 * Common routines: these are in user.c
 */

void Attach_User_Context(struct Kernel_Thread* kthread, struct User_Context* context);
void Detach_User_Context(struct Kernel_Thread* kthread);
int Spawn(const char *program, const char *command, struct Kernel_Thread **pThread);
void Switch_To_User_Context(struct Kernel_Thread* kthread, struct Interrupt_State* state);

/*
 * Implementation routines: these are in userseg.c or uservm.c
 */

void Destroy_User_Context(struct User_Context* context);
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext);
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize);
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize);
void Switch_To_Address_Space(struct User_Context *userContext);


#endif  /* GEEKOS_USER_H */
