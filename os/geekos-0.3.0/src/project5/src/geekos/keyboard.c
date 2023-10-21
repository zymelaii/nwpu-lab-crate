/*
 * Keyboard driver
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.14 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * Information sources:
 * - Chapter 8 of _The Undocumented PC_, 2nd ed, by Frank van Gilluwe,
 *   ISBN 0-201-47950-8.
 * - Pages 400-409 of _The Programmers PC Sourcebook_, by Thom Hogan,
 *   ISBN 1-55615-118-7.
 */

/*
 * Credits:
 * - Peter Gnodde <peter@pcswebdesign.nl> added support for
 *   the CTRL and ALT modifiers
 */

/*
 * TODO list:
 * - Right now we're assuming an 83-key keyboard.
 *   Should add support for 101+ keyboards.
 * - Should toggle keyboard LEDs.
 */

#include <geekos/kthread.h>
#include <geekos/kassert.h>
#include <geekos/screen.h>
#include <geekos/irq.h>
#include <geekos/io.h>
#include <geekos/keyboard.h>

/* ----------------------------------------------------------------------
 * Private data and functions
 * ---------------------------------------------------------------------- */

/*
 * Current shift state.
 */
#define LEFT_SHIFT  0x01
#define RIGHT_SHIFT 0x02
#define LEFT_CTRL   0x04
#define RIGHT_CTRL  0x08
#define LEFT_ALT    0x10
#define RIGHT_ALT   0x20
#define SHIFT_MASK  (LEFT_SHIFT | RIGHT_SHIFT)
#define CTRL_MASK   (LEFT_CTRL | RIGHT_CTRL)
#define ALT_MASK    (LEFT_ALT | RIGHT_ALT)
static unsigned s_shiftState = 0;

/*
 * Queue for keycodes, in case they arrive faster than consumer
 * can deal with them.
 */
#define QUEUE_SIZE 256
#define QUEUE_MASK 0xff
#define NEXT(index) (((index) + 1) & QUEUE_MASK)
static Keycode s_queue[QUEUE_SIZE];
static int s_queueHead, s_queueTail;

/*
 * Wait queue for thread(s) waiting for keyboard events.
 */
static struct Thread_Queue s_waitQueue;

/*
 * Translate from scan code to key code, when shift is not pressed.
 */
static const Keycode s_scanTableNoShift[] = {
    KEY_UNKNOWN, ASCII_ESC, '1', '2',   /* 0x00 - 0x03 */
    '3', '4', '5', '6',                 /* 0x04 - 0x07 */
    '7', '8', '9', '0',                 /* 0x08 - 0x0B */
    '-', '=', ASCII_BS, '\t',           /* 0x0C - 0x0F */
    'q', 'w', 'e', 'r',                 /* 0x10 - 0x13 */
    't', 'y', 'u', 'i',                 /* 0x14 - 0x17 */
    'o', 'p', '[', ']',                 /* 0x18 - 0x1B */
    '\r', KEY_LCTRL, 'a', 's',          /* 0x1C - 0x1F */
    'd', 'f', 'g', 'h',                 /* 0x20 - 0x23 */
    'j', 'k', 'l', ';',                 /* 0x24 - 0x27 */
    '\'', '`', KEY_LSHIFT, '\\',        /* 0x28 - 0x2B */
    'z', 'x', 'c', 'v',                 /* 0x2C - 0x2F */
    'b', 'n', 'm', ',',                 /* 0x30 - 0x33 */
    '.', '/', KEY_RSHIFT, KEY_PRINTSCRN, /* 0x34 - 0x37 */
    KEY_LALT, ' ', KEY_CAPSLOCK, KEY_F1, /* 0x38 - 0x3B */
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,     /* 0x3C - 0x3F */
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,     /* 0x40 - 0x43 */
    KEY_F10, KEY_NUMLOCK, KEY_SCRLOCK, KEY_KPHOME,  /* 0x44 - 0x47 */
    KEY_KPUP, KEY_KPPGUP, KEY_KPMINUS, KEY_KPLEFT,  /* 0x48 - 0x4B */
    KEY_KPCENTER, KEY_KPRIGHT, KEY_KPPLUS, KEY_KPEND,  /* 0x4C - 0x4F */
    KEY_KPDOWN, KEY_KPPGDN, KEY_KPINSERT, KEY_KPDEL,  /* 0x50 - 0x53 */
    KEY_SYSREQ, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,  /* 0x54 - 0x57 */
};
#define SCAN_TABLE_SIZE (sizeof(s_scanTableNoShift) / sizeof(Keycode))

/*
 * Translate from scan code to key code, when shift *is* pressed.
 * Keep this in sync with the unshifted table above!
 * They must be the same size.
 */
static const Keycode s_scanTableWithShift[] = {
    KEY_UNKNOWN, ASCII_ESC, '!', '@',   /* 0x00 - 0x03 */
    '#', '$', '%', '^',                 /* 0x04 - 0x07 */
    '&', '*', '(', ')',                 /* 0x08 - 0x0B */
    '_', '+', ASCII_BS, '\t',           /* 0x0C - 0x0F */
    'Q', 'W', 'E', 'R',                 /* 0x10 - 0x13 */
    'T', 'Y', 'U', 'I',                 /* 0x14 - 0x17 */
    'O', 'P', '{', '}',                 /* 0x18 - 0x1B */
    '\r', KEY_LCTRL, 'A', 'S',          /* 0x1C - 0x1F */
    'D', 'F', 'G', 'H',                 /* 0x20 - 0x23 */
    'J', 'K', 'L', ':',                 /* 0x24 - 0x27 */
    '"', '~', KEY_LSHIFT, '|',          /* 0x28 - 0x2B */
    'Z', 'X', 'C', 'V',                 /* 0x2C - 0x2F */
    'B', 'N', 'M', '<',                 /* 0x30 - 0x33 */
    '>', '?', KEY_RSHIFT, KEY_PRINTSCRN, /* 0x34 - 0x37 */
    KEY_LALT, ' ', KEY_CAPSLOCK, KEY_F1, /* 0x38 - 0x3B */
    KEY_F2, KEY_F3, KEY_F4, KEY_F5,     /* 0x3C - 0x3F */
    KEY_F6, KEY_F7, KEY_F8, KEY_F9,     /* 0x40 - 0x43 */
    KEY_F10, KEY_NUMLOCK, KEY_SCRLOCK, KEY_KPHOME,  /* 0x44 - 0x47 */
    KEY_KPUP, KEY_KPPGUP, KEY_KPMINUS, KEY_KPLEFT,  /* 0x48 - 0x4B */
    KEY_KPCENTER, KEY_KPRIGHT, KEY_KPPLUS, KEY_KPEND,  /* 0x4C - 0x4F */
    KEY_KPDOWN, KEY_KPPGDN, KEY_KPINSERT, KEY_KPDEL,  /* 0x50 - 0x53 */
    KEY_SYSREQ, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,  /* 0x54 - 0x57 */
};

static __inline__ bool Is_Queue_Empty(void)
{
    return s_queueHead == s_queueTail;
}

static __inline__ bool Is_Queue_Full(void)
{
    return NEXT(s_queueTail) == s_queueHead;
}

static __inline__ void Enqueue_Keycode(Keycode keycode)
{
    if (!Is_Queue_Full()) {
	s_queue[ s_queueTail ] = keycode;
	s_queueTail = NEXT(s_queueTail);
    }
}

static __inline__ Keycode Dequeue_Keycode(void)
{
    Keycode result;
    KASSERT(!Is_Queue_Empty());
    result = s_queue[ s_queueHead ];
    s_queueHead = NEXT(s_queueHead);
    return result;
}

/*
 * Handler for keyboard interrupts.
 */
static void Keyboard_Interrupt_Handler(struct Interrupt_State* state)
{
    uchar_t status, scanCode;
    unsigned flag = 0;
    bool release = false, shift;
    Keycode keycode;

    Begin_IRQ(state);

    status = In_Byte(KB_CMD);
    IO_Delay();

    if ((status & KB_OUTPUT_FULL) != 0) {
	/* There is a byte available */
	scanCode = In_Byte(KB_DATA);
	IO_Delay();
/*
 *	Print("code=%x%s\n", scanCode, (scanCode&0x80) ? " [release]" : "");
 */

	if (scanCode & KB_KEY_RELEASE) {
	    release = true;
	    scanCode &= ~(KB_KEY_RELEASE);
	}

	if (scanCode >= SCAN_TABLE_SIZE) {
	    Print("Unknown scan code: %x\n", scanCode);
	    goto done;
	}

	/* Process the key */
	shift = ((s_shiftState & SHIFT_MASK) != 0);
	keycode = shift ? s_scanTableWithShift[scanCode] : s_scanTableNoShift[scanCode];

	/* Update shift, control and alt state */
	switch (keycode) {
	case KEY_LSHIFT:
	    flag = LEFT_SHIFT;
	    break;
	case KEY_RSHIFT:
	    flag = RIGHT_SHIFT;
	    break;
	case KEY_LCTRL:
	    flag = LEFT_CTRL;
	    break;
	case KEY_RCTRL:
	    flag = RIGHT_CTRL;
	    break;
	case KEY_LALT:
	    flag = LEFT_ALT;
	    break;
	case KEY_RALT:
	    flag = RIGHT_ALT;
	    break;
	default:
	    goto noflagchange;
	}

	if (release)
	    s_shiftState &= ~(flag);
	else
	    s_shiftState |= flag;
			
	/*
	 * Shift, control and alt keys don't have to be
	 * queued, flags will be set!
	 */
	goto done;

noflagchange:
	/* Format the new keycode */
	if (shift)
	    keycode |= KEY_SHIFT_FLAG;
	if ((s_shiftState & CTRL_MASK) != 0)
	    keycode |= KEY_CTRL_FLAG;
	if ((s_shiftState & ALT_MASK) != 0)
	    keycode |= KEY_ALT_FLAG;
	if (release)
	    keycode |= KEY_RELEASE_FLAG;
		
	/* Put the keycode in the buffer */
	Enqueue_Keycode(keycode);

	/* Wake up event consumers */
	Wake_Up(&s_waitQueue);

	/*
	 * Pick a new thread upon return from interrupt
	 * (hopefully the one waiting for the keyboard event)
	 */
	g_needReschedule = true;
    }

done:
    End_IRQ(state);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

void Init_Keyboard(void)
{
    ushort_t irqMask;

    Print("Initializing keyboard...\n");

    /* Start out with no shift keys enabled. */
    s_shiftState = 0;

    /* Buffer is initially empty. */
    s_queueHead = s_queueTail = 0;

    /* Install interrupt handler */
    Install_IRQ(KB_IRQ, Keyboard_Interrupt_Handler);

    /* Enable IRQ1 (keyboard) */
    irqMask = Get_IRQ_Mask();
    irqMask &= ~(1 << KB_IRQ);
    Set_IRQ_Mask(irqMask);
}

/*
 * Poll for a key event.
 * Returns true if a key is available,
 * false if not.  If a key event is available,
 * it will be stored in the location pointed to
 * by keycode.
 */
bool Read_Key(Keycode* keycode)
{
    bool result, iflag;

    iflag = Begin_Int_Atomic();

    result = !Is_Queue_Empty();
    if (result) {
	*keycode = Dequeue_Keycode();
    }

    End_Int_Atomic(iflag);

    return result;
}

/*
 * Wait for a keycode to arrive.
 * Uses the keyboard wait queue to sleep until
 * a keycode arrives.
 */
Keycode Wait_For_Key(void)
{
    bool gotKey, iflag;
    Keycode keycode = KEY_UNKNOWN;

    iflag = Begin_Int_Atomic();

    do {
	gotKey = !Is_Queue_Empty();
	if (gotKey)
	    keycode = Dequeue_Keycode();
	else
	    Wait(&s_waitQueue);
    }
    while (!gotKey);

    End_Int_Atomic(iflag);

    return keycode;
}
