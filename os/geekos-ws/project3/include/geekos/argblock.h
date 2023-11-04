/*
 * Create and extract the command line argument block for a process
 * Copyright (c) 2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.8 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_ARGBLOCK_H
#define GEEKOS_ARGBLOCK_H

/**
 * Header struct for accessing argument block from user mode.
 * Just cast the address of the argument block passed by
 * the kernel to a pointer to this struct.
 */
struct Argument_Block {
    int argc;
    char **argv;
};

#ifdef GEEKOS

/*
 * Functions used by the kernel to create the argument block.
 */
void Get_Argument_Block_Size(const char *command, unsigned *numArgs, ulong_t *argBlockSize);
void Format_Argument_Block(char *argBlock, unsigned numArgs, ulong_t userAddress,
    const char *command);

#endif

#endif  /* GEEKOS_ARGBLOCK_H */
