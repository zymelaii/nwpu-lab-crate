/*
 * Synchronization primitives
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/kthread.h>
#include <geekos/int.h>
#include <geekos/kassert.h>
#include <geekos/screen.h>
#include <geekos/synch.h>

/*
 * NOTES:
 * - The GeekOS mutex and condition variable APIs are based on those
 *   in pthreads.
 * - Unlike disabling interrupts, mutexes offer NO protection against
 *   concurrent execution of interrupt handlers.  Mutexes and
 *   condition variables should only be used from kernel threads,
 *   with interrupts enabled.
 */

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

/*
 * The mutex is currently locked.
 * Atomically reenable preemption and wait in the
 * mutex's wait queue.
 */
static void Mutex_Wait(struct Mutex *mutex)
{
    KASSERT(mutex->state == MUTEX_LOCKED);
    KASSERT(g_preemptionDisabled);

    Disable_Interrupts();
    g_preemptionDisabled = false;
    Wait(&mutex->waitQueue);
    g_preemptionDisabled = true;
    Enable_Interrupts();
}

/*
 * Lock given mutex.
 * Preemption must be disabled.
 */
static __inline__ void Mutex_Lock_Imp(struct Mutex* mutex)
{
    KASSERT(g_preemptionDisabled);

    /* Make sure we're not already holding the mutex */
    KASSERT(!IS_HELD(mutex));

    /* Wait until the mutex is in an unlocked state */
    while (mutex->state == MUTEX_LOCKED) {
	Mutex_Wait(mutex);
    }

    /* Now it's ours! */
    mutex->state = MUTEX_LOCKED;
    mutex->owner = g_currentThread;
}

/*
 * Unlock given mutex.
 * Preemption must be disabled.
 */
static __inline__ void Mutex_Unlock_Imp(struct Mutex* mutex)
{
    KASSERT(g_preemptionDisabled);

    /* Make sure mutex was actually acquired by this thread. */
    KASSERT(IS_HELD(mutex));

    /* Unlock the mutex. */
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;

    /*
     * If there are threads waiting to acquire the mutex,
     * wake one of them up.  Note that it is legal to inspect
     * the queue with interrupts enabled because preemption
     * is disabled, and therefore we know that no thread can
     * concurrently add itself to the queue.
     */
    if (!Is_Thread_Queue_Empty(&mutex->waitQueue)) {
	Disable_Interrupts();
	Wake_Up_One(&mutex->waitQueue);
	Enable_Interrupts();
    }
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize given mutex.
 */
void Mutex_Init(struct Mutex* mutex)
{
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;
    Clear_Thread_Queue(&mutex->waitQueue);
}

/*
 * Lock given mutex.
 */
void Mutex_Lock(struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    g_preemptionDisabled = true;
    Mutex_Lock_Imp(mutex);
    g_preemptionDisabled = false;
}

/*
 * Unlock given mutex.
 */
void Mutex_Unlock(struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    g_preemptionDisabled = true;
    Mutex_Unlock_Imp(mutex);
    g_preemptionDisabled = false;
}

/*
 * Initialize given condition.
 */
void Cond_Init(struct Condition* cond)
{
    Clear_Thread_Queue(&cond->waitQueue);
}

/*
 * Wait on given condition (protected by given mutex).
 */
void Cond_Wait(struct Condition* cond, struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    /* Ensure mutex is held. */
    KASSERT(IS_HELD(mutex));

    /* Turn off scheduling. */
    g_preemptionDisabled = true;

    /*
     * Release the mutex, but leave preemption disabled.
     * No other threads will be able to run before this thread
     * is able to wait.  Therefore, this thread will not
     * miss the eventual notification on the condition.
     */
    Mutex_Unlock_Imp(mutex);

    /*
     * Atomically reenable preemption and wait in the condition wait queue.
     * Other threads can run while this thread is waiting,
     * and eventually one of them will call Cond_Signal() or Cond_Broadcast()
     * to wake up this thread.
     * On wakeup, disable preemption again.
     */
    Disable_Interrupts();
    g_preemptionDisabled = false;
    Wait(&cond->waitQueue);
    g_preemptionDisabled = true;
    Enable_Interrupts();

    /* Reacquire the mutex. */
    Mutex_Lock_Imp(mutex);

    /* Turn scheduling back on. */
    g_preemptionDisabled = false;
}

/*
 * Wake up one thread waiting on the given condition.
 * The mutex guarding the condition should be held!
 */
void Cond_Signal(struct Condition* cond)
{
    KASSERT(Interrupts_Enabled());
    Disable_Interrupts();  /* prevent scheduling */
    Wake_Up_One(&cond->waitQueue);
    Enable_Interrupts();  /* resume scheduling */
}

/*
 * Wake up all threads waiting on the given condition.
 * The mutex guarding the condition should be held!
 */
void Cond_Broadcast(struct Condition* cond)
{
    KASSERT(Interrupts_Enabled());
    Disable_Interrupts();  /* prevent scheduling */
    Wake_Up(&cond->waitQueue);
    Enable_Interrupts();  /* resume scheduling */
}
