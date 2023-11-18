/*
 * Keyboard driver
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_KEYBOARD_H
#define GEEKOS_KEYBOARD_H

#include <geekos/ktypes.h>

/* ----------------------------------------------------------------------
 * Hardware stuff
 * ---------------------------------------------------------------------- */

#ifdef GEEKOS

#define KB_IRQ 1

/*
 * I/O ports
 */
#define KB_CMD 0x64
#define KB_DATA 0x60

/*
 * Bits in status port
 */
#define KB_OUTPUT_FULL 0x01

/*
 * High bit in scan code is set when key is released
 */
#define KB_KEY_RELEASE 0x80

#endif  /* GEEKOS */

/* ----------------------------------------------------------------------
 * Software keycodes
 * ---------------------------------------------------------------------- */

/*
 * Each keyboard event generates a 16 bit code.
 * - The low 10 bits indicate which key was used.
 * - If bit 8 (KEY_SPECIAL_FLAG) is 0, then the low 8 bits contain
 *   the ASCII code.
 * - The flags indicate the shift/alt/control state,
 *   and whether the event was a make or release.
 */

typedef ushort_t Keycode;

/*
 * Flags
 */
#define KEY_SPECIAL_FLAG 0x0100
#define KEY_KEYPAD_FLAG  0x0200
#define KEY_SHIFT_FLAG   0x1000
#define KEY_ALT_FLAG     0x2000
#define KEY_CTRL_FLAG    0x4000
#define KEY_RELEASE_FLAG 0x8000

/*
 * Special key codes
 */
#define _SPECIAL(num) (KEY_SPECIAL_FLAG | (num))
#define KEY_UNKNOWN _SPECIAL(0)
#define KEY_F1      _SPECIAL(1)
#define KEY_F2      _SPECIAL(2)
#define KEY_F3      _SPECIAL(3)
#define KEY_F4      _SPECIAL(4)
#define KEY_F5      _SPECIAL(5)
#define KEY_F6      _SPECIAL(6)
#define KEY_F7      _SPECIAL(7)
#define KEY_F8      _SPECIAL(8)
#define KEY_F9      _SPECIAL(9)
#define KEY_F10     _SPECIAL(10)
#define KEY_F11     _SPECIAL(11)
#define KEY_F12     _SPECIAL(12)
#define KEY_LCTRL   _SPECIAL(13)
#define KEY_RCTRL   _SPECIAL(14)
#define KEY_LSHIFT  _SPECIAL(15)
#define KEY_RSHIFT  _SPECIAL(16)
#define KEY_LALT    _SPECIAL(17)
#define KEY_RALT    _SPECIAL(18)
#define KEY_PRINTSCRN _SPECIAL(19)
#define KEY_CAPSLOCK _SPECIAL(20)
#define KEY_NUMLOCK _SPECIAL(21)
#define KEY_SCRLOCK _SPECIAL(22)
#define KEY_SYSREQ  _SPECIAL(23)

/*
 * Keypad keys
 */
#define KEYPAD_START 128
#define _KEYPAD(num) (KEY_KEYPAD_FLAG | KEY_SPECIAL_FLAG | (num+KEYPAD_START))
#define KEY_KPHOME  _KEYPAD(0)
#define KEY_KPUP    _KEYPAD(1)
#define KEY_KPPGUP  _KEYPAD(2)
#define KEY_KPMINUS _KEYPAD(3)
#define KEY_KPLEFT  _KEYPAD(4)
#define KEY_KPCENTER _KEYPAD(5)
#define KEY_KPRIGHT _KEYPAD(6)
#define KEY_KPPLUS  _KEYPAD(7)
#define KEY_KPEND   _KEYPAD(8)
#define KEY_KPDOWN  _KEYPAD(9)
#define KEY_KPPGDN  _KEYPAD(10)
#define KEY_KPINSERT _KEYPAD(11)
#define KEY_KPDEL   _KEYPAD(12)

/*
 * ASCII codes for which there is no convenient C representation
 */
#define ASCII_ESC 0x1B
#define ASCII_BS  0x08

#ifdef GEEKOS

/*
 * Public functions
 */
void Init_Keyboard(void);
bool Read_Key(Keycode* keycode);
Keycode Wait_For_Key(void);

#endif  /* GEEKOS */

#endif  /* GEEKOS_KEYBOARD_H */
