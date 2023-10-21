/*
 * This is the device-driver interface to the interrupt system.
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.12 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/kassert.h>
#include <geekos/idt.h>
#include <geekos/io.h>
#include <geekos/irq.h>

/* ----------------------------------------------------------------------
 * Private functions and data
 * ---------------------------------------------------------------------- */

/*
 * Current IRQ mask.
 * This should be kept up to date with setup.asm
 * (which does the initial programming of the PICs).
 */
static ushort_t s_irqMask = 0xfffb;

/*
 * Get the master and slave parts of an IRQ mask.
 */
#define MASTER(mask) ((mask) & 0xff)
#define SLAVE(mask) (((mask)>>8) & 0xff)


/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Install a handler for given IRQ.
 * Note that we don't unmask the IRQ.
 */
void Install_IRQ(int irq, Interrupt_Handler handler)
{
    Install_Interrupt_Handler(irq + FIRST_EXTERNAL_INT, handler);
}

/*
 * Get current IRQ mask.  Each bit position represents
 * one of the 16 IRQ lines.
 */
ushort_t Get_IRQ_Mask(void)
{
    return s_irqMask;
}

/*
 * Set the IRQ mask.
 */
void Set_IRQ_Mask(ushort_t mask)
{
    uchar_t oldMask, newMask;

    oldMask = MASTER(s_irqMask);
    newMask = MASTER(mask);
    if (newMask != oldMask) {
	Out_Byte(0x21, newMask);
    }

    oldMask = SLAVE(s_irqMask);
    newMask = SLAVE(mask);
    if (newMask != oldMask) {
	Out_Byte(0xA1, newMask);
    }

    s_irqMask = mask;
}

/*
 * Enable given IRQ.
 */
void Enable_IRQ(int irq)
{
    bool iflag = Begin_Int_Atomic();

    KASSERT(irq >= 0 && irq < 16);
    ushort_t mask = Get_IRQ_Mask();
    mask &= ~(1 << irq);
    Set_IRQ_Mask(mask);

    End_Int_Atomic(iflag);
}

/*
 * Disable given IRQ.
 */
void Disable_IRQ(int irq)
{
    bool iflag = Begin_Int_Atomic();

    KASSERT(irq >= 0 && irq < 16);
    ushort_t mask = Get_IRQ_Mask();
    mask |= (1 << irq);
    Set_IRQ_Mask(mask);

    End_Int_Atomic(iflag);
}

/*
 * Called by an IRQ handler to begin the interrupt.
 * Currently a no-op.
 */
void Begin_IRQ(struct Interrupt_State* state)
{
}

/*
 * Called by an IRQ handler to end the interrupt.
 * Sends an EOI command to the appropriate PIC(s).
 */
void End_IRQ(struct Interrupt_State* state)
{
    int irq = state->intNum - FIRST_EXTERNAL_INT;
    uchar_t command = 0x60 | (irq & 0x7);

    if (irq < 8) {
	/* Specific EOI to master PIC */
	Out_Byte(0x20, command);
    }
    else {
	/* Specific EOI to slave PIC, then to master (cascade line) */
	Out_Byte(0xA0, command);
	Out_Byte(0x20, 0x62);
    }
}
