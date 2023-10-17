/*
 * Curses emulation for GeekOS
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.3 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * This module emulates just enough of the curses API
 * to run the ae editor.
 */

#ifndef CURSES_H
#define CURSES_H

#include <geekos/ktypes.h>

struct _Window;
typedef struct _Window WINDOW;

extern WINDOW *stdscr;

#define OK 0
extern int LINES;
extern int COLS;

int clear(void);
int getch(void);
int move(int y, int x);
int addch(int c);
int clrtoeol(void);
int clrtobot(void);
int mvaddstr(int y, int x, const char *s);
int refresh(void);
WINDOW *initscr(void);
int raw(void);
int noecho(void);
int idlok(WINDOW *w, bool bf);
int endwin(void);

#endif /* CURSES_H */

