; Low level interrupt/thread handling code for GeekOS.
; Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
; Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
; $Revision: 1.15 $

; This is free software.  You are permitted to use,
; redistribute, and modify it as specified in the file "COPYING".

; This is 32 bit code to be linked into the kernel.
; It defines low level interrupt handler entry points that we'll use
; to populate the IDT.  It also contains the interrupt handling
; and thread context switch code.

%include "defs.asm"
%include "symbol.asm"

[BITS 32]

; ----------------------------------------------------------------------
; Definitions
; ----------------------------------------------------------------------

; This is the size of the Interrupt_State struct in int.h
INTERRUPT_STATE_SIZE equ 64

; Save registers prior to calling a handler function.
; This must be kept up to date with:
;   - Interrupt_State struct in int.h
;   - Setup_Initial_Thread_Context() in kthread.c
%macro Save_Registers 0
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	push	fs
	push	gs
%endmacro

; Restore registers and clean up the stack after calling a handler function
; (i.e., just before we return from the interrupt via an iret instruction).
%macro Restore_Registers 0
	pop	gs
	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	add	esp, 8	; skip int num and error code
%endmacro


; Number of bytes between the top of the stack and
; the interrupt number after the general-purpose and segment
; registers have been saved.
REG_SKIP equ (11*4)

; Template for entry point code for interrupts that have
; an explicit processor-generated error code.
; The argument is the interrupt number.
%macro Int_With_Err 1
align 8
	push	dword %1	; push interrupt number
	jmp	Handle_Interrupt ; jump to common handler
%endmacro

; Template for entry point code for interrupts that do not
; generate an explicit error code.  We push a dummy error
; code on the stack, so the stack layout is the same
; for all interrupts.
%macro Int_No_Err 1
align 8
	push	dword 0		; fake error code
	push	dword %1	; push interrupt number
	jmp	Handle_Interrupt ; jump to common handler
%endmacro


; ----------------------------------------------------------------------
; Symbol imports and exports
; ----------------------------------------------------------------------

; This symbol is defined in idt.c, and is a table of addresses
; of C handler functions for interrupts.
IMPORT g_interruptTable

; Global variable pointing to context struct for current thread.
IMPORT g_currentThread

; Set to non-zero when we need to choose a new thread
; in the interrupt return code.
IMPORT g_needReschedule

; Set to non-zero when preemption is disabled.
IMPORT g_preemptionDisabled

; This is the function that returns the next runnable thread.
IMPORT Get_Next_Runnable

; Function to put a thread on the run queue.
IMPORT Make_Runnable


; Sizes of interrupt handler entry points for interrupts with
; and without error codes.  The code in idt.c uses this
; information to infer the layout of the table of interrupt
; handler entry points, without needing a separate linker
; symbol for each one (which is quite tedious to type :-)
EXPORT g_handlerSizeNoErr
EXPORT g_handlerSizeErr

; Simple functions to load the IDTR, GDTR, and LDTR.
EXPORT Load_IDTR
EXPORT Load_GDTR
EXPORT Load_LDTR

; Beginning and end of the table of interrupt entry points.
EXPORT g_entryPointTableStart
EXPORT g_entryPointTableEnd

; Thread context switch function.
EXPORT Switch_To_Thread

; Return current value of eflags register.
EXPORT Get_Current_EFLAGS


; ----------------------------------------------------------------------
; Code
; ----------------------------------------------------------------------

[SECTION .text]

; Load IDTR with 6-byte pointer whose address is passed as
; the parameter.
align 8
Load_IDTR:
	mov	eax, [esp+4]
	lidt	[eax]
	ret

;  Load the GDTR with 6-byte pointer whose address is
; passed as the parameter.  Assumes that interrupts
; are disabled.
align 8
Load_GDTR:
	mov	eax, [esp+4]
	lgdt	[eax]
	; Reload segment registers
	mov	ax, KERNEL_DS
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	jmp	KERNEL_CS:.here
.here:
	ret

; Load the LDT whose selector is passed as a parameter.
align 8
Load_LDTR:
	mov	eax, [esp+4]
	lldt	ax
	ret

; Common interrupt handling code.
; Save registers, call C handler function,
; possibly choose a new thread to run, restore
; registers, return from the interrupt.
align 8
Handle_Interrupt:
	; Save registers (general purpose and segment)
	Save_Registers

	; Ensure that we're using the kernel data segment
	mov	ax, KERNEL_DS
	mov	ds, ax
	mov	es, ax

	; Get the address of the C handler function from the
	; table of handler functions.
	mov	eax, g_interruptTable	; get address of handler table
	mov	esi, [esp+REG_SKIP]	; get interrupt number
	mov	ebx, [eax+esi*4]	; get address of handler function

	; Call the handler.
	; The argument passed is a pointer to an Interrupt_State struct,
	; which describes the stack layout for all interrupts.
	push	esp
	call	ebx
	add	esp, 4			; clear 1 argument

	; If preemption is disabled, then the current thread
	; keeps running.
	cmp	[g_preemptionDisabled], dword 0
	jne	.restore

	; See if we need to choose a new thread to run.
	cmp	[g_needReschedule], dword 0
	je	.restore

	; Put current thread back on the run queue
	push	dword [g_currentThread]
	call	Make_Runnable
	add	esp, 4			; clear 1 argument

	; Save stack pointer in current thread context, and
	; clear numTicks field.
	mov	eax, [g_currentThread]
	mov	[eax+0], esp		; esp field
	mov	[eax+4], dword 0	; numTicks field

	; Pick a new thread to run, and switch to its stack
	call	Get_Next_Runnable
	mov	[g_currentThread], eax
	mov	esp, [eax+0]		; esp field

	; Clear "need reschedule" flag
	mov	[g_needReschedule], dword 0

.restore:

	; Restore registers
	Restore_Registers

	; Return from the interrupt.
	iret

; ----------------------------------------------------------------------
; Switch_To_Thread()
;   Save context of currently executing thread, and activate
;   the thread whose context object is passed as a parameter.
; 
; Parameter: 
;   - ptr to Kernel_Thread whose state should be restored and made active
;
; Notes:
; Called with interrupts disabled.
; This must be kept up to date with definition of Kernel_Thread
; struct, in kthread.h.
; ----------------------------------------------------------------------
align 16
Switch_To_Thread:
	; Modify the stack to allow a later return via an iret instruction.
	; We start with a stack that looks like this:
	;
	;            thread_ptr
	;    esp --> return addr
	;
	; We change it to look like this:
	;
	;            thread_ptr
	;            eflags
	;            cs
	;    esp --> return addr

	push	eax		; save eax
	mov	eax, [esp+4]	; get return address
	mov	[esp-4], eax	; move return addr down 8 bytes from orig loc
	add	esp, 8		; move stack ptr up
	pushfd			; put eflags where return address was
	mov	eax, [esp-4]	; restore saved value of eax
	push	dword KERNEL_CS	; push cs selector
	sub	esp, 4		; point stack ptr at return address

	; Push fake error code and interrupt number
	push	dword 0
	push	dword 0

	; Save general purpose registers.
	Save_Registers

	; Save stack pointer in the thread context struct (at offset 0).
	mov	eax, [g_currentThread]
	mov	[eax+0], esp

	; Clear numTicks field in thread context, since this
	; thread is being suspended.
	mov	[eax+4], dword 0

	; Load the pointer to the new thread context into eax.
	; We skip over the Interrupt_State struct on the stack to
	; get the parameter.
	mov	eax, [esp+INTERRUPT_STATE_SIZE]

	; Make the new thread current, and switch to its stack.
	mov	[g_currentThread], eax
	mov	esp, [eax+0]


	; Restore general purpose and segment registers, and clear interrupt
	; number and error code.
	Restore_Registers

	; We'll return to the place where the thread was
	; executing last.
	iret

; Return current contents of eflags register.
align 16
Get_Current_EFLAGS:
	pushfd			; push eflags
	pop	eax		; pop contents into eax
	ret

; ----------------------------------------------------------------------
; Generate interrupt-specific entry points for all interrupts.
; We also define symbols to indicate the extend of the table
; of entry points, and the size of individual entry points.
; ----------------------------------------------------------------------
align 8
g_entryPointTableStart:

; Handlers for processor-generated exceptions, as defined by
; Intel 486 manual.
Int_No_Err 0
align 8
Before_No_Err:
Int_No_Err 1
align 8
After_No_Err:
Int_No_Err 2	; FIXME: not described in 486 manual
Int_No_Err 3
Int_No_Err 4
Int_No_Err 5
Int_No_Err 6
Int_No_Err 7
align 8
Before_Err:
Int_With_Err 8
align 8
After_Err:
Int_No_Err 9	; FIXME: not described in 486 manual
Int_With_Err 10
Int_With_Err 11
Int_With_Err 12
Int_With_Err 13
Int_With_Err 14
Int_No_Err 15	; FIXME: not described in 486 manual
Int_No_Err 16
Int_With_Err 17

; The remaining interrupts (18 - 255) do not have error codes.
; We can generate them all in one go with nasm's %rep construct.
%assign intNum 18
%rep (256 - 18)
Int_No_Err intNum
%assign intNum intNum+1
%endrep

align 8
g_entryPointTableEnd:

[SECTION .data]

; Exported symbols defining the size of handler entry points
; (both with and without error codes).
align 4
g_handlerSizeNoErr: dd (After_No_Err - Before_No_Err)
align 4
g_handlerSizeErr: dd (After_Err - Before_Err)
