/*
 * Kernel-mode Console I/O
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.25 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */
void ELF_Print(char* msg)
{

  __asm__ __volatile__ (
                        "int $0x90"
                        : /* no return */
                        : "a" (msg)     // goes in eax
                        );
}
