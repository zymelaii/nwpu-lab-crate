/*
 * Create and extract the command line argument block for a process
 * Copyright (c) 2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.14 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/string.h>
#include <geekos/argblock.h>
/*#include <geekos/screen.h> */

/**
 * Is given character a space?
 * @param c the character
 */
static bool Is_Space(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/**
 * Skip whitespace in given string.
 * @param s the string
 * @return same string with leading whitespace skipped
 */
static const char *Skip_Whitespace(const char *s)
{
    while (Is_Space(*s))
	++s;
    return s;
}

/**
 * Get the length of the first argument token in given string.
 * @param arg the string
 * @return number of characters in the argument token
 */
static unsigned Get_Argument_Len(const char *arg)
{
    const char *s = arg;
    unsigned len = 0;

    while (*s != '\0' && !Is_Space(*s)) {
	++len;
	++s;
    }
    return len;
}

/**
 * Determine the buffer size and number of arguments needed
 * to format an argument block for the given command.
 *
 * @param command a command string
 * @param numArgs pointer to int where number of arguments will be returned
 * @param argBlockSize pointer to int where buffer size of argument block
 *   will be returned
 */
void Get_Argument_Block_Size(const char *command, unsigned *numArgs, ulong_t *argBlockSize)
{
    ulong_t size = 0;
    unsigned argCount = 0;
    const char *s = command;

    size += sizeof(int);  /* Argument count */
    size += sizeof(char **);  /* Pointer to argument vector */

    /*
     * Tokenize the command string, keeping track of the length
     * of each token.
     */
    for (;;) {
	int len;

	s = Skip_Whitespace(s);
	/*Print("%s\n", s); */
	if (*s == '\0')
	    break;
	len = Get_Argument_Len(s);
	s += len;

	size += sizeof(char *);  /* Pointer to the argument (in argv) */
	size += len + 1;  /* Buffer storing the argument, with nul terminator */
	++argCount;
    }

    /* argv[] is terminated by a null pointer */
    size += sizeof(char *);

    *numArgs = argCount;
    *argBlockSize = size;

    /*Print("argCount=%d\n", argCount); */
}

/**
 * Format a user process command line argument block.
 * The Get_Argument_Block_Size() function must be called first
 * to determine the amount of memory required for the argument
 * block, and to determine how many arguments there are.
 *
 * @param argBlock pointer to kernel buffer where argument block is being built
 * @param numArgs number of command line arguments
 * @param userAddress the address where the argument block will
 *   be located in user mode
 * @param command the command used to build the argument block
 */
void Format_Argument_Block(char *argBlock, unsigned numArgs, ulong_t userAddress,
    const char *command)
{
    unsigned len;
    ulong_t *argv;
    char *dst = argBlock;
    const char *s = command;

    /* Store argc in first word (argc field of Argument_Block) */
    *(int *)dst = numArgs;
    dst += sizeof(int);

    /*
     * Store pointer to argument vector array
     * (argv field of Argument_Block)
     */
    *(ulong_t *)dst = userAddress + sizeof(struct Argument_Block);
    dst += sizeof(char **);

    /* Argv array immediately follows argv pointer */
    argv = (ulong_t *) dst;
    dst += (numArgs+1) * sizeof(char *);

    /* The argument strings are located immediately after argv */
    for (;;) {
	s = Skip_Whitespace(s);
	if (*s == '\0')
	    break;
	*argv++ = userAddress + (dst - argBlock);  /* Set argv element */

	len = Get_Argument_Len(s);
	memcpy(dst, s, len);  /* Copy argument into arg block */
	dst += len;
	*dst++ = '\0';  /* Nul-terminate the argument */

	s += len;
    }

    /* Argv terminated by null pointer */
    *argv++ = 0;
}
