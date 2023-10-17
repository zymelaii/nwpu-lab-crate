; GeekOS setup code
; Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
; $Revision: 1.10 $

; This is free software.  You are permitted to use,
; redistribute, and modify it as specified in the file "COPYING".

; A lot of this code is adapted from Kernel Toolkit 0.2
; and Linux version 2.2.x, so the following copyrights apply:

; Copyright (C) 1991, 1992 Linus Torvalds
; modified by Drew Eckhardt
; modified by Bruce Evans (bde)
; adapted for Kernel Toolkit by Luigi Sgro

%include "defs.asm"

[BITS 16]
[ORG 0x0]

start_setup:

	; Redefine the data segment so we can access variables
	; declared in this file.
	mov	ax, SETUPSEG
	mov	ds, ax

	; Use int 15h to find out size of extended memory in KB.
	; Extended memory is the memory above 1MB.  So by
	; adding 1MB to this amount, we get the total amount
	; of system memory.  We can only detect 64MB this way,
	; but that's OK for now.
	mov	ah, 0x88
	int	0x15
	add	ax, 1024	; 1024 KB == 1 MB
	mov	[mem_size_kbytes], ax

	; Kill the floppy motor.
	call	Kill_Motor

	; Block interrupts, since we can't meaningfully handle them yet
	; and we no longer need BIOS services.
	cli

	; Set up IDT and GDT registers
	lidt	[IDT_Pointer]
	lgdt	[GDT_Pointer]

	; Initialize the interrupt controllers, and enable the
	; A20 address line
	call	Init_PIC
	call	Enable_A20

	; Switch to protected mode!
	mov	ax, 0x01
	lmsw	ax

	; Jump to 32 bit code.
	jmp	dword KERNEL_CS:(SETUPSEG << 4) + setup_32

[BITS 32]
setup_32:

	; set up data segment registers
	mov	ax, KERNEL_DS
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	; Create the stack for the initial kernel thread.
	mov	esp, KERN_STACK + 4096

	; Build Boot_Info struct on stack.
	; Note that we push the fields on in reverse order,
	; since the stack grows downwards.
	xor	eax, eax
	mov	ax, [(SETUPSEG<<4)+mem_size_kbytes]
	push	eax		; memSizeKB
	push	dword 8		; bootInfoSize

	; Pass pointer to Boot_Info struct as argument to kernel
	; entry point.
	push	esp

	; Push return address to make this look like a call
	; XXX - untested
	push	dword (SETUPSEG<<4)+.returnAddr

	; Far jump into kernel
	jmp	KERNEL_CS:ENTRY_POINT

.returnAddr:
	; We shouldn't return here.
.here:	jmp .here

[BITS 16]

; Kill the floppy motor.
; This code was shamelessly stolen from Linux.
Kill_Motor:
	mov	dx, 0x3f2
	xor	al, al
	out	dx, al
	ret

Init_PIC:
	; Initialize master and slave PIC!
	mov	al, ICW1
	out	0x20, al		; ICW1 to master
	call	Delay
	out	0xA0, al		; ICW1 to slave
	call	Delay
	mov	al, ICW2_MASTER
	out	0x21, al		; ICW2 to master
	call	Delay
	mov	al, ICW2_SLAVE
	out	0xA1, al		; ICW2 to slave
	call	Delay
	mov	al, ICW3_MASTER
	out	0x21, al		; ICW3 to master
	call	Delay
	mov	al, ICW3_SLAVE
	out	0xA1, al		; ICW3 to slave
	call	Delay
	mov	al, ICW4
	out	0x21, al		; ICW4 to master
	call	Delay
	out	0xA1, al		; ICW4 to slave
	call	Delay
	mov	al, 0xff		; mask all ints in slave
	out	0xA1, al		; OCW1 to slave
	call	Delay
	mov	al, 0xfb		; mask all ints but 2 in master
	out	0x21, al		; OCW1 to master
	call	Delay
	ret

; Linux uses this code.
; The idea is that some systems issue port I/O instructions
; faster than the device hardware can deal with them.
Delay:
	jmp	.done
.done:	ret

; Enable the A20 address line, so we can correctly address
; memory above 1MB.
Enable_A20:
	mov	al, 0xD1
	out	0x64, al
	call	Delay
	mov	al, 0xDF
	out	0x60, al
	call	Delay
	ret


; ----------------------------------------------------------------------
; Setup data
; ----------------------------------------------------------------------

mem_size_kbytes: dw 0


; ----------------------------------------------------------------------
; The GDT.  Creates flat 32-bit address space for the kernel
; code, data, and stack.  Note that this GDT is just used
; to create an environment where we can start running 32 bit
; code.  The kernel will create and manage its own GDT.
; ----------------------------------------------------------------------

; GDT initialization stuff
NUM_GDT_ENTRIES equ 3		; number of entries in GDT
GDT_ENTRY_SZ equ 8		; size of a single GDT entry

align 8, db 0
GDT:
	; Descriptor 0 is not used
	dw 0
	dw 0
	dw 0
	dw 0

	; Descriptor 1: kernel code segment
	dw 0xFFFF	; bytes 0 and 1 of segment size
	dw 0x0000	; bytes 0 and 1 of segment base address
	db 0x00		; byte 2 of segment base address
	db 0x9A		; present, DPL=0, non-system, code, non-conforming,
			;   readable, not accessed
	db 0xCF		; granularity=page, 32 bit code, upper nibble of size
	db 0x00		; byte 3 of segment base address

	; Descriptor 2: kernel data and stack segment
	; NOTE: what Intel calls an "expand-up" segment
	; actually means that the stack will grow DOWN,
	; towards lower memory.  So, we can use this descriptor
	; for both data and stack references.
	dw 0xFFFF	; bytes 0 and 1 of segment size
	dw 0x0000	; bytes 0 and 1 of segment base address
	db 0x00		; byte 2 of segment base address
	db 0x92		; present, DPL=0, non-system, data, expand-up,
			;   writable, not accessed
	db 0xCF		; granularity=page, big, upper nibble of size
	db 0x00		; byte 3 of segment base address

GDT_Pointer:
	dw NUM_GDT_ENTRIES*GDT_ENTRY_SZ	; limit
	dd (SETUPSEG<<4) + GDT		; base address

IDT_Pointer:
	dw 0
	dd 00
