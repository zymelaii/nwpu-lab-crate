/*
 * GeekOS text screen output
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.15 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_SCREEN_H
#define GEEKOS_SCREEN_H

#include <geekos/ktypes.h>

#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define AMBER   6
#define GRAY    7
#define BRIGHT  8
#define ATTRIB(bg,fg) ((fg)|((bg)<<4))

#define NUMCOLS 80
#define NUMROWS 25

#define TABWIDTH 8

#ifdef GEEKOS

/*
 * VGA hardware stuff, for accessing the text display
 * memory and controlling the cursor
 */
#define VIDMEM_ADDR 0xb8000
#define VIDMEM ((uchar_t*) VIDMEM_ADDR)
#define CRT_ADDR_REG 0x3D4
#define CRT_DATA_REG 0x3D5
#define CRT_CURSOR_LOC_HIGH_REG 0x0E
#define CRT_CURSOR_LOC_LOW_REG 0x0F

void Init_Screen(void);
void Clear_Screen(void);
void Get_Cursor(int* row, int* col);
bool Put_Cursor(int row, int col);
uchar_t Get_Current_Attr(void);
void Set_Current_Attr(uchar_t attrib);
void Put_Char(int c);
void Put_String(const char* s);
void Put_Buf(const char* buf, ulong_t length);
void Print(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

#endif  /* GEEKOS */

#endif  /* GEEKOS_SCREEN_H */
