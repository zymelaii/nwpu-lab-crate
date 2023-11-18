/*
 * Synchronization primitives
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_SYNCH_H
#define GEEKOS_SYNCH_H

#include <geekos/kthread.h>
#define MAX_REGISTERED_THREADS 60
#define MAX_SEMAPHORE_NAME 25

/*  
* 信号量结构体定义  
*/
struct Semaphore
{
    int semaphoreID;                                                 /* 信号量的 ID */
    char semaphoreName[MAX_SEMAPHORE_NAME + 1];                      /* 信号量的名字(以'\0'结尾) */
    int value;                                                       /* 信号量的值 */
    int registeredThreadCount;                                       /* 注册该信号量的线程数量 */
    struct Kernel_Thread *registeredThreads[MAX_REGISTERED_THREADS]; /* 注册的线程 */
    struct Thread_Queue waitingThreads;                              /* 等待该信号的线程队列 */
    DEFINE_LINK(Semaphore_List, Semaphore);                          /* 连接信号链表的指针域 */
};
/* 宏定义：定义双向链表(/include/geekos/list.h) */
DEFINE_LIST(Semaphore_List, Semaphore);
typedef struct Semaphore *pSemaphore;
IMPLEMENT_LIST(Semaphore_List, Semaphore);

/* 函数声明 */
int Create_Semaphore(char *semName, int nameLen, int initCount);
int P(int sid);
int V(int sid);
int Destroy_Semaphore(int sid);

/*
 * mutex states
 */
enum
{
    MUTEX_UNLOCKED,
    MUTEX_LOCKED
};

struct Mutex
{
    int state;
    struct Kernel_Thread *owner;
    struct Thread_Queue waitQueue;
};

#define MUTEX_INITIALIZER                           \
    {                                               \
        MUTEX_UNLOCKED, 0, THREAD_QUEUE_INITIALIZER \
    }

struct Condition
{
    struct Thread_Queue waitQueue;
};

void Mutex_Init(struct Mutex *mutex);
void Mutex_Lock(struct Mutex *mutex);
void Mutex_Unlock(struct Mutex *mutex);

void Cond_Init(struct Condition *cond);
void Cond_Wait(struct Condition *cond, struct Mutex *mutex);
void Cond_Signal(struct Condition *cond);
void Cond_Broadcast(struct Condition *cond);

#define IS_HELD(mutex) \
    ((mutex)->state == MUTEX_LOCKED && (mutex)->owner == g_currentThread)

#endif /* GEEKOS_SYNCH_H */
