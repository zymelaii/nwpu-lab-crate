/*
 * x86 port IO routines
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.10 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/io.h>

/*
 * Write a byte to an I/O port.
 */
void Out_Byte(ushort_t port, uchar_t value)
{
    __asm__ __volatile__ (
	"outb %b0, %w1"
	:
	: "a" (value), "Nd" (port)
    );
}

/*
 * Read a byte from an I/O port.
 */
uchar_t In_Byte(ushort_t port)
{
    uchar_t value;

    __asm__ __volatile__ (
	"inb %w1, %b0"
	: "=a" (value)
	: "Nd" (port)
    );

    return value;
}

/*
 * Write a word to an I/O port.
 */
void Out_Word(ushort_t port, ushort_t value)
{
    __asm__ __volatile__ (
	"outw %w0, %w1"
	:
	: "a" (value), "Nd" (port)
    );
}

/*
 * Read a byte from an I/O port.
 */
ushort_t In_Word(ushort_t port)
{
    ushort_t value;

    __asm__ __volatile__ (
	"inw %w1, %w0"
	: "=a" (value)
	: "Nd" (port)
    );

    return value;
}

/*
 * Short delay.  May be needed when talking to some
 * (slow) I/O devices.
 */
void IO_Delay(void)
{
    uchar_t value = 0;
    __asm__ __volatile__ (
	"outb %0, $0x80"
	:
	: "a" (value)
    );
}
