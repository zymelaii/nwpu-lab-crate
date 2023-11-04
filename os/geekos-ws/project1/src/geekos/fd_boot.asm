; Boot sector for GeekOS
; Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
; Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
; $Revision: 1.5 $

; This is free software.  You are permitted to use,
; redistribute, and modify it as specified in the file "COPYING".

; Loads setup code and a program image from sectors 1..n of a floppy
; and executes the setup code (which will in turn execute
; the program image).

; Some of this code is adapted from Kernel Toolkit 0.2
; and Linux version 2.2.x, so the following copyrights apply:

; Copyright (C) 1991, 1992 Linus Torvalds
; modified by Drew Eckhardt
; modified by Bruce Evans (bde)
; adapted for Kernel Toolkit by Luigi Sgro

%include "defs.asm"

; Pad to desired offset from start symbol.
;    Usage: Pad_From_Symbol offset, symbol
%macro Pad_From_Symbol 2
	times (%1 - ($ - %2)) db 0
%endmacro

; ----------------------------------------------------------------------
; The actual code
; ----------------------------------------------------------------------

[BITS 16]
[ORG 0x0]

BeginText:	; needed to calculate padding bytes to fill the sector

	; Copy the boot sector into INITSEG.
	mov	ax, BOOTSEG
	mov	ds, ax			; source segment for string copy
	xor	si, si			; source index for string copy
	mov	ax, INITSEG
	mov	es, ax			; destination segment for string copy
	xor	di, di			; destination index for string copy
	cld				; clear direction flag
	mov	cx, 256			; number of words to copy
	rep	movsw			; copy 512 bytes

	jmp	INITSEG:after_move

after_move:
	; Now we're executing in INITSEG

	; We want the data segment to refer to INITSEG
	; (since we've defined variables in the same place as the code)
	mov	ds, ax			; ax still contains INITSEG

	; Put the stack in the place where we were originally loaded.
	; By definition, there is nothing important there now.
	mov	ax, 0
	mov	ss, ax
	mov	sp, (BOOTSEG << 4) + 512 - 2

load_setup:
	; Load the setup code.
	mov	ax, word [setupStart]
	mov	word [sec_count], ax
	add	ax, [setupSize]
	mov	word [max_sector], ax
.again:
	mov	ax, [sec_count]
	push	ax			; 1st param to ReadSector (log sec num)
	push	word SETUPSEG		; 2nd param to ReadSector (seg base)
	sub	ax, [setupStart]	; convert to 0-indexed
	shl	ax, 9			; multiply by 512
	push	ax			;  ...to get 3rd param (byte offset)

	; read the sector from the floppy
	call	ReadSector
	add	sp, 6			; clear 3 word params

	; on to next sector
	inc	word [sec_count]

	; are we done?
	mov	bx, word [max_sector]
	cmp	word [sec_count], bx
	jl	.again

load_kernel:
	; Load the kernel image from sectors KERN_START_SEC..n of the
	; floppy into memory at KERNSEG.  Note that there are 128 sectors
	; per 64K segment.  So, when figuring out which segment to
	; load the sector into, we shift right by 7 bits (which is
	; equivalent to dividing by 128).

	; Figure out start sector and max sector
	mov	ax, word [kernelStart]
	mov	word [sec_count], ax
	add	ax, word [kernelSize]
	mov	word [max_sector], ax
.again:
	mov	ax, [sec_count]		; logical sector on the floppy
	push	ax			; 1st param to ReadSector (log sec num)
	sub	ax, [kernelStart]	; convert to 0-indexed
	mov	cx, ax			; save in cx
	shr	ax, 7			; divide by 128
	shl	ax, 12			;  ...and multiply by 0x1000
	add	ax, KERNSEG		;  ...to get base relative to KERNSEG
	push	ax			; 2nd param to ReadSector (seg base)
	and	cx, 0x7f		; mod sector by 128
	shl	cx, 9			;  ...and multiply by 512
	push	cx			; to get offset in segment (3rd parm)

	; read the sector from the floppy
	call	ReadSector
	add	sp, 6			; clear 3 word params

	; on to next sector
	inc	word [sec_count]

	; have we loaded all of the sectors?
	mov	bx, word [max_sector]
	cmp	word [sec_count], bx
	jl	.again

	; Now we've loaded the setup code and the kernel image.
	; Jump to setup code.
	jmp	SETUPSEG:0

; Read a sector from the floppy drive.
; This code (and the rest of this boot sector) will have to
; be re-written at some point so it reads more than one
; sector at a time.
;
; Parameters:
;     - "logical" sector number   [bp+8]
;     - destination segment       [bp+6]
;     - destination offset        [bp+4]
ReadSector:
	push	bp			; set up stack frame
	mov	bp, sp			; "
	pusha				; save all registers

%if 0
; debug params
	mov	dx, [bp+8]
	call	PrintHex
	call	PrintNL
	mov	dx, [bp+6]
	call	PrintHex
	call	PrintNL
	mov	dx, [bp+4]
	call	PrintHex
	call	PrintNL
%endif

	; Sector = log_sec % SECTORS_PER_TRACK
	; Head = (log_sec / SECTORS_PER_TRACK) % HEADS
	mov	ax, [bp+8]		; get logical sector number from stack
	xor	dx, dx			; dx is high part of dividend (== 0)
	mov	bx, SECTORS_PER_TRACK	; divisor
	div	bx			; do the division
	mov	[sec], dx		; sector is the remainder
	and	ax, 1			; same as mod by HEADS==2 (slight hack)
	mov	[head], ax

	; Track = log_sec / (SECTORS_PER_TRACK*HEADS)
	mov	ax, [bp+8]		; get logical sector number again
	xor	dx, dx			; dx is high part of dividend
	mov	bx, SECTORS_PER_TRACK*2	; divisor
	div	bx			; do the division
	mov	[track], ax		; track is quotient

%if 0
; debugging code
	mov	dx, [sec]
	call	PrintHex
	call	PrintNL
	mov	dx, [head]
	call	PrintHex
	call	PrintNL
	mov	dx, [track]
	call	PrintHex
	call	PrintNL
%endif

	; Now, try to actually read the sector from the floppy,
	; retrying up to 3 times.

	mov	[num_retries], byte 0

.again:
	mov	ax, [bp+6]		; dest segment...
	mov	es, ax			;   goes in es
	mov	ax, (0x02 << 8) | 1	; function = 02h in ah,
					;   # secs = 1 in al
	mov	bx, [track]		; track number...
	mov	ch, bl			;   goes in ch
	mov	bx, [sec]		; sector number...
	mov	cl, bl			;   goes in cl...
	inc	cl			;   but it must be 1-based, not 0-based
	mov	bx, [head]		; head number...
	mov	dh, bl			;   goes in dh
	xor	dl, dl			; hard code drive=0
	mov	bx, [bp+4]		; offset goes in bx
					;   (es:bx points to buffer)

	; Call the BIOS Read Diskette Sectors service
	int	0x13

	; If the carry flag is NOT set, then there was no error
	; and we're done.
	jnc	.done

	; Error - code stored in ah
	mov	dx, ax
	call PrintHex
	inc	byte [num_retries]
	cmp	byte [num_retries], 3
	jne	.again

	; If we got here, we failed thrice, so we give up
	mov	dx, 0xdead
	call	PrintHex
.here:	jmp	.here

.done:
	popa				; restore all regisiters
	pop	bp			; leave stack frame
	ret

; Include utility routines
%include "util.asm"

; ----------------------------------------------------------------------
; Variables
; ----------------------------------------------------------------------

; These are used by ReadSector
head: dw 0
track: dw 0
sec: dw 0
num_retries: db 0

; Used for loops reading sectors from floppy
sec_count: dw 0
max_sector: dw 0

; Padding to make the PFAT Boot Record sit just before the BIOS signature.
Pad_From_Symbol PFAT_BOOT_RECORD_OFFSET, BeginText

; PFAT boot record
; Describes how to load the setup program and kernel.
; The default values are appropriate for creating a boot
; floppy by concatenating the boot sector, setup program,
; and kernel image.  The buildFat program will change
; these values if the boot floppy is formatted as a PFAT
; filesystem.
	dw	0
	dw	0

	dw	0
	dw	0

	dw	0
	dw	0

	dw	0
	dw	0

	dw	0
	dw	0

;; part of pfat boot record
setupStart:		
	dw	1			; by default, setup is at first sector	

;; part of pfat boot record
setupSize:
	dw	NUM_SETUP_SECTORS	; number of sectors in setup

;; part of pfat boot record
kernelStart:
	dw	1+NUM_SETUP_SECTORS	; default start sector for kernel

;; part of pfat boot record
kernelSize:
	dw	NUM_KERN_SECTORS	; number of sectors in kernel

; Finish by writing the BIOS signature to mark this as
; a valid boot sector.
Pad_From_Symbol BIOS_SIGNATURE_OFFSET, BeginText
Signature   dw 0xAA55   ; BIOS controls this to ensure this is a boot sector
