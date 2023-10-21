; This code is adapted from Kernel Toolkit 0.2
; and Linux version 2.2.x, so the following copyrights apply:

; Copyright (C) 1991, 1992 Linus Torvalds
; modified by Drew Eckhardt
; modified by Bruce Evans (bde)
; adapted for Kernel Toolkit by Luigi Sgro

%ifndef UTIL_ASM
%define UTIL_ASM

; The following were copied from ktk-0.2 bootsect.asm, and were presumably
; from the Linux bootsect code.  I changed them a little so they
; don't clobber the caller's registers.

; Print the word contained in the dx register to the screen.
PrintHex:
	pusha
	mov   cx, 4      	; 4 hex digits
.PrintDigit:
	rol   dx, 4      	; rotate so that lowest 4 bits are used
   	mov   ax, 0E0Fh		; ah = request, al = mask for nybble
   	and   al, dl
   	add   al, 90h		; convert al to ascii hex (four instructions)
   	daa			; I've spent 1 hour to understand how it works..
   	adc   al, 40h
   	daa
   	int   10h
   	loop  .PrintDigit
	popa
   	ret

; Print a newline.
PrintNL:			; print CR and NL
	push	ax
	mov	ax, 0E0Dh	; CR
       	int	10h
       	mov	al, 0Ah		; LF
       	int	10h
	pop	ax
       	ret

%endif
