
; Low level interrupt/thread handling code for GeekOS.
; Copyright (c) 2004 David H. Hovemeyer <daveho@cs.umd.edu>
; Copyright (c) 2004 Iulian Neamtiu <neamtiu@cs.umd.edu> 

; $Revision: 1.1 $

; This is free software.  You are permitted to use,
; redistribute, and modify it as specified in the file "COPYING".


; Dirty code to run an executable in kernel mode. 

; You shouldn't normally change this file !

	
; Trampoline gets called with two selectors (code/data)
; and the entry address. We push the CS,DS,IP and do a 'retf'
; to force an inter-selector jump to the new program.
; When the new program is done, the leave/lret in entry.c
; will bring us back here. 
;

; ----------------------------------------------------------------------
; low-level spawn stuff
; ----------------------------------------------------------------------

%include "symbol.asm"
%include "defs.asm"
[BITS 32]

; ----------------------------------------------------------------------
; Definitions
; ----------------------------------------------------------------------
; 
EXPORT Trampoline


; ----------------------------------------------------------------------
; Code
; ----------------------------------------------------------------------

[SECTION .text]

align 8
Trampoline:
;; first we fetch the code selector off the stack	
	mov	ebx, [esp+4]
;; then we fetch the data selector off the stack	
	mov	eax, [esp+8]
;; and finally the entry address	
	mov	ecx, [esp+0xc]

	push    ds	
	push    es	
	mov	ds, ax
	mov	es, ax

;; push KERNEL_CS/EIP so that we return here
;; after running the program
	push    dword KERNEL_CS	
	push    dword .backhere
	
;; now make the inter-selector jump
;; we land in different cs/ds descriptors 
	push    ebx
	push    ecx
	retf		
.backhere:

	pop es
	pop ds
 	ret
	
