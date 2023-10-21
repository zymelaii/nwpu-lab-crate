/*
 * GeekOS timer interrupt support
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * $Revision: 1.22 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <limits.h>
#include <geekos/io.h>
#include <geekos/int.h>
#include <geekos/irq.h>
#include <geekos/kthread.h>
#include <geekos/timer.h>


/*
 * Global tick counter
 */
volatile ulong_t g_numTicks;

/*
 * Number of times the spin loop can execute during one timer tick
 */
static int s_spinCountPerTick;

/*
 * Number of ticks to wait before calibrating the delay loop.
 */
#define CALIBRATE_NUM_TICKS	3

/*
 * The default quantum; maximum number of ticks a thread can use before
 * we suspend it and choose another.
 */
#define DEFAULT_MAX_TICKS 4

/*
 * Settable quantum.
 */
int g_Quantum = DEFAULT_MAX_TICKS;

/*
 * Ticks per second.
 * FIXME: should set this to something more reasonable, like 100.
 */
#define TICKS_PER_SEC 18

/*#define DEBUG_TIMER */
#ifdef DEBUG_TIMER
#  define Debug(args...) Print(args)
#else
#  define Debug(args...)
#endif

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

static void Timer_Interrupt_Handler(struct Interrupt_State* state)
{
    struct Kernel_Thread* current = g_currentThread;

    Begin_IRQ(state);

    /* Update global and per-thread number of ticks */
    ++g_numTicks;
    ++current->numTicks;


    /*
     * If thread has been running for an entire quantum,
     * inform the interrupt return code that we want
     * to choose a new thread.
     */
    if (current->numTicks >= g_Quantum) {
	g_needReschedule = true;
    }


    End_IRQ(state);
}

/*
 * Temporary timer interrupt handler used to calibrate
 * the delay loop.
 */
static void Timer_Calibrate(struct Interrupt_State* state)
{
    Begin_IRQ(state);
    if (g_numTicks < CALIBRATE_NUM_TICKS)
	++g_numTicks;
    else {
	/*
	 * Now we can look at EAX, which reflects how many times
	 * the loop has executed
	 */
	/*Print("Timer_Calibrate: eax==%d\n", state->eax);*/
	s_spinCountPerTick = INT_MAX  - state->eax;
	state->eax = 0;  /* make the loop terminate */
    }
    End_IRQ(state);
}

/*
 * Delay loop; spins for given number of iterations.
 */
static void Spin(int count)
{
    /*
     * The assembly code is the logical equivalent of
     *      while (count-- > 0) { // waste some time }
     * We rely on EAX being used as the counter
     * variable.
     */

    int result;
    __asm__ __volatile__ (
	"1: decl %%eax\n\t"
	"cmpl $0, %%eax\n\t"
	"nop; nop; nop; nop; nop; nop\n\t"
	"nop; nop; nop; nop; nop; nop\n\t"
	"jg 1b"
	: "=a" (result)
	: "a" (count)
    );
}

/*
 * Calibrate the delay loop.
 * This will initialize s_spinCountPerTick, which indicates
 * how many iterations of the loop are executed per timer tick.
 */
static void Calibrate_Delay(void)
{
    Disable_Interrupts();

    /* Install temporarily interrupt handler */
    Install_IRQ(TIMER_IRQ, &Timer_Calibrate);
    Enable_IRQ(TIMER_IRQ);

    Enable_Interrupts();

    /* Wait a few ticks */
    while (g_numTicks < CALIBRATE_NUM_TICKS)
	;

    /*
     * Execute the spin loop.
     * The temporary interrupt handler will overwrite the
     * loop counter when the next tick occurs.
     */
    Spin(INT_MAX);

    Disable_Interrupts();

    /*
     * Mask out the timer IRQ again,
     * since we will be installing a real timer interrupt handler.
     */
    Disable_IRQ(TIMER_IRQ);
    Enable_Interrupts();
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

void Init_Timer(void)
{
    /*
     * TODO: reprogram the timer to set the frequency.
     * In bochs, it defaults to 18Hz, which is actually pretty
     * reasonable.
     */

    Print("Initializing timer...\n");


    /* Calibrate for delay loop */
    Calibrate_Delay();
    Print("Delay loop: %d iterations per tick\n", s_spinCountPerTick);

    /* Install an interrupt handler for the timer IRQ */
    Install_IRQ(TIMER_IRQ, &Timer_Interrupt_Handler);
    Enable_IRQ(TIMER_IRQ);
}


#define US_PER_TICK (TICKS_PER_SEC * 1000000)

/*
 * Spin for at least given number of microseconds.
 * FIXME: I'm sure this implementation leaves a lot to
 * be desired.
 */
void Micro_Delay(int us)
{
    int num = us * s_spinCountPerTick;
    int denom = US_PER_TICK;

    int numSpins = num / denom;
    int rem = num % denom;

    if (rem > 0)
	++numSpins;

    Debug("Micro_Delay(): num=%d, denom=%d, spin count = %d\n", num, denom, numSpins);

    Spin(numSpins);
}
