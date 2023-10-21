/*
 * GeekOS interrupt handling data structures and functions
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.17 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/idt.h>	 /* x86-specific int handling stuff */
#include <geekos/screen.h>
#include <geekos/kassert.h>
#include <geekos/int.h>

/*
 * Defined in lowlevel.asm.
 */
ulong_t Get_Current_EFLAGS(void);

/* ----------------------------------------------------------------------
 * Private functions and data
 * ---------------------------------------------------------------------- */

/*
 * A dummy interrupt handler function.
 * Ensures that the low-level interrupt code always
 * has a handler to call.
 */
static void Dummy_Interrupt_Handler(struct Interrupt_State* state)
{
    Print("*** Unexpected interrupt! ***\n");
    Dump_Interrupt_State(state);
    STOP();
}

static void Print_Selector(const char* regName, uint_t value)
{
    Print("%s: index=%d, ti=%d, rpl=%d\n",
	regName, value >> 3, (value >> 2) & 1, value & 3);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize the interrupt system.
 */
void Init_Interrupts(void)
{
    int i;

    /* Low-level initialization.  Build and initialize the IDT. */
    Init_IDT();

    /*
     * Initialize all entries of the handler table with a dummy handler.
     * This will ensure that we always have a handler function to call.
     */
    for (i = 0; i < NUM_IDT_ENTRIES; ++i) {
	Install_Interrupt_Handler(i, Dummy_Interrupt_Handler);
    }

    /* Re-enable interrupts */
    Enable_Interrupts();
}

/*
 * Query whether or not interrupts are currently enabled.
 */
bool Interrupts_Enabled(void)
{
    ulong_t eflags = Get_Current_EFLAGS();
    return (eflags & EFLAGS_IF) != 0;
}

/*
 * Dump interrupt state struct to screen
 */
void Dump_Interrupt_State(struct Interrupt_State* state)
{
    uint_t errorCode = state->errorCode;

    Print("eax=%08x ebx=%08x ecx=%08x edx=%08x\n"
	   "esi=%08x edi=%08x ebp=%08x\n"
	   "eip=%08x cs=%08x eflags=%08x\n"
	   "Interrupt number=%d, error code=%d\n"
	   "index=%d, TI=%d, IDT=%d, EXT=%d\n",
	state->eax, state->ebx, state->ecx, state->edx,
	state->esi, state->edi, state->ebp,
	state->eip, state->cs, state->eflags,
	state->intNum, errorCode,
	errorCode >> 3, (errorCode >> 2) & 1, (errorCode >> 1) & 1, errorCode & 1
    );
    if (Is_User_Interrupt(state)) {
	struct User_Interrupt_State *ustate = (struct User_Interrupt_State*) state;
	Print("user esp=%08x, user ss=%08x\n", ustate->espUser, ustate->ssUser);
    }
    Print_Selector("cs", state->cs);
    Print_Selector("ds", state->ds);
    Print_Selector("es", state->es);
    Print_Selector("fs", state->fs);
    Print_Selector("gs", state->gs);
}
