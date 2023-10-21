/*
 * Kernel threads
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.30 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_KTHREAD_H
#define GEEKOS_KTHREAD_H

#include <geekos/ktypes.h>
#include <geekos/list.h>

struct Kernel_Thread;
struct User_Context;
struct Interrupt_State;

/*
 * Queue of threads.
 * This is used for the run queue(s), and also for
 * thread synchronization and communication.
 */
DEFINE_LIST(Thread_Queue, Kernel_Thread);

/*
 * List which includes all threads.
 */
DEFINE_LIST(All_Thread_List, Kernel_Thread);

/*
 * Kernel thread context data structure.
 * NOTE: there is assembly code in lowlevel.asm that depends
 * on the offsets of the fields in this struct, so if you change
 * the layout, make sure everything gets updated.
 */
struct Kernel_Thread {
    ulong_t esp;			 /* offset 0 */
    volatile ulong_t numTicks;		 /* offset 4 */
    int priority;
    DEFINE_LINK(Thread_Queue, Kernel_Thread);
    void* stackPage;
    struct User_Context* userContext;
    struct Kernel_Thread* owner;
    int refCount;

    /* These fields are used to implement the Join() function */
    bool alive;
    struct Thread_Queue joinQueue;
    int exitCode;

    /* The kernel thread id; also used as process id */
    int pid;

    /* Link fields for list of all threads in the system. */
    DEFINE_LINK(All_Thread_List, Kernel_Thread);

    /* Array of MAX_TLOCAL_KEYS pointers to thread-local data. */
#define MAX_TLOCAL_KEYS 128
    const void* tlocalData[MAX_TLOCAL_KEYS];

    /*
     * The run queue level that the thread should be put on
     * when it is restarted.
     */
    int currentReadyQueue;
    bool blocked;
};

/*
 * Define Thread_Queue and All_Thread_List access and manipulation functions.
 */
IMPLEMENT_LIST(Thread_Queue, Kernel_Thread);
IMPLEMENT_LIST(All_Thread_List, Kernel_Thread);

static __inline__ void Enqueue_Thread(struct Thread_Queue *queue, struct Kernel_Thread *kthread) {
    Add_To_Back_Of_Thread_Queue(queue, kthread);
}

static __inline__ void Remove_Thread(struct Thread_Queue *queue, struct Kernel_Thread *kthread) {
    Remove_From_Thread_Queue(queue, kthread);
}

/*
 * Thread start functions should have this signature.
 */
typedef void (*Thread_Start_Func)(ulong_t arg);

/*
 * Thread priorities
 */
#define PRIORITY_IDLE    0
#define PRIORITY_USER    1
#define PRIORITY_LOW     2
#define PRIORITY_NORMAL  5
#define PRIORITY_HIGH   10

/*
 * Number of ready queue levels.
 */
#define MAX_QUEUE_LEVEL 4

/*
 * Scheduler operations.
 */
void Init_Scheduler(void);
struct Kernel_Thread* Start_Kernel_Thread(
    Thread_Start_Func startFunc,
    ulong_t arg,
    int priority,
    bool detached
);
struct Kernel_Thread* Start_User_Thread(struct User_Context* userContext, bool detached);
void Make_Runnable(struct Kernel_Thread* kthread);
void Make_Runnable_Atomic(struct Kernel_Thread* kthread);
struct Kernel_Thread* Get_Current(void);
struct Kernel_Thread* Get_Next_Runnable(void);
void Schedule(void);
void Yield(void);
void Exit(int exitCode) __attribute__ ((noreturn));
int Join(struct Kernel_Thread* kthread);
struct Kernel_Thread* Lookup_Thread(int pid);

/*
 * Thread context switch function, defined in lowlevel.asm
 */
void Switch_To_Thread(struct Kernel_Thread*);

/*
 * Wait queue functions.
 */
void Wait(struct Thread_Queue* waitQueue);
void Wake_Up(struct Thread_Queue* waitQueue);
void Wake_Up_One(struct Thread_Queue* waitQueue);

/*
 * Pointer to currently executing thread.
 */
extern struct Kernel_Thread* g_currentThread;

/*
 * Boolean flag indicating that we need to choose a new runnable thread.
 */
extern int g_needReschedule;

/*
 * Boolean flag indicating that preemption should be disabled.
 */
extern volatile int g_preemptionDisabled;

/*
 * Thread-local data information
 */
#define MIN_DESTRUCTOR_ITERATIONS 4

typedef void (*tlocal_destructor_t)(void *);
typedef unsigned int tlocal_key_t;

extern int Tlocal_Create(tlocal_key_t *, tlocal_destructor_t);
extern void Tlocal_Put(tlocal_key_t, const void *);
extern void *Tlocal_Get(tlocal_key_t);

/* Print list of all threads, for debugging. */
extern void Dump_All_Thread_List(void);


#endif  /* GEEKOS_KTHREAD_H */
