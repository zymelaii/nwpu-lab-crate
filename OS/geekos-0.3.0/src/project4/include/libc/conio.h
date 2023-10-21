/*
 * User-mode Console I/O
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.18 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef CONIO_H
#define CONIO_H

#include <stddef.h>
#include <geekos/ktypes.h>
#include <geekos/keyboard.h>	 /* key codes */
#include <geekos/screen.h>	 /* screen attributes */

void Print(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
int Print_String(const char* msg);
int Put_Char(int ch);
Keycode Get_Key(void);
int Set_Attr(int attr);
int Get_Cursor(int *row, int *col);
int Put_Cursor(int row, int col);

void Echo(bool enable);
void Read_Line(char* buf, size_t bufSize);

const char *Get_Error_String(int errno);

#define assert(exp)						\
do {								\
    if (!(exp)) {						\
	extern int Exit(int);					\
	Print("\x1b[1;37;41m"					\
	    "Failed assertion: %s: at %s, line %d\x1B[37;40m\n",\
	    #exp, __FILE__, __LINE__);				\
	Exit(1);						\
    }								\
} while (0)

#endif  /* CONIO_H */

