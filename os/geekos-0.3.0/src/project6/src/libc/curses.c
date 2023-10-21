/*
 * Curses emulation for GeekOS
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.3 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <limits.h>
#include <conio.h>
#include <process.h>
#include <fileio.h>
#include <curses.h>

/*
 * Notes:
 * - We can generally avoid redrawing parts of the screen
 *   that haven't been modified, even when the app eagerly redraws
 *   things (the "ae" editor does this)
 * - Scrolling in untested, but should work (I think)
 *
 * TODO:
 * - Buffer the output in refresh(), to avoid making one system
 *   call for each output character.
 * - Eliminate struct Screen - we should use two WINDOW objects
 *   (which is what real curses does)
 */

/* ----------------------------------------------------------------------
 * Private functions and data
 * ---------------------------------------------------------------------- */

#define MAXLINES 25
#define MAXCOLS 80

/* End of line marker */
#define EOL 0

struct Line {
    int buf[MAXCOLS];
    bool modified;
};

struct Screen {
    struct Line lines[MAXLINES];
    int y, x;
};

struct _Window {
    struct Screen display;	/* The real screen image */
    struct Screen work;		/* How we want the screen to appear */
    int nScroll;		/* Num lines scrolled since last update */
};

#define WORK(pwin) (&(pwin)->work)
#define DISPLAY(pwin) (&(pwin)->display)
#define X(scr) ((scr)->x)
#define Y(scr) ((scr)->y)
#define LINE(scr,y)  (&((scr)->lines[(y)]))
#define CURLINE(scr) LINE(scr,Y(scr))

static WINDOW s_stdscr;

static __inline__ int Move_Coord(int c, int limit)
{
    if (c < 0)
	return 0;
    if (c >= limit)
	return limit - 1;
    return c;
}

static void Scroll_Screen(struct Screen *s)
{
    int j;
    struct Line *last = LINE(s, LINES-1);
    for (j = 1; j < LINES; ++j)
	*(LINE(s, j-1)) = *(LINE(s, j));
    last->buf[0] = EOL;
    last->modified = false;
}

static void Scroll_Win(WINDOW *w)
{
    Scroll_Screen(WORK(w));
    ++w->nScroll;
}

static void Paint_Char(struct Line *line, int x, int c)
{
    int orig = line->buf[x];
    line->buf[x] = c;
    if (c != orig)
	line->modified = true;
}

/*
 * Set a character or end of line marker without
 * updating the cursor.  Updates end of line marker
 * if required.
 */
static void Set_Char(WINDOW *w, int y, int x, int c)
{
    struct Screen *s = WORK(w);
    struct Line *line = LINE(s, y);
    int i, eol = INT_MAX;

    for (i = 0; i < COLS; ++i) {
	if (line->buf[i] == EOL) {
	    eol = i;
	    break;
	}
    }

    for (i = eol; i < x; ++i)
	Paint_Char(line,i,' ');

    Paint_Char(line,x,c);

    if (eol < x+1 && x < LINES-1)
	Paint_Char(line,x+1,EOL);
}

/*
 * New line, and return to column 0.
 */
static void Newline(WINDOW *w)
{
    struct Screen *s = WORK(w);
    Set_Char(w,Y(s),X(s),EOL);
    if (Y(s) == LINES-1)
	Scroll_Win(w);
    else
	Y(s) = Y(s) + 1;
    X(s) = 0;
}

/*
 * Set a character at current cursor position,
 * updating the cursor and scrolling as required.
 */
static void Add_Char(WINDOW *w, int c)
{
    struct Screen *s = WORK(w);

    if (c == '\n') {
	Newline(w);
    } else if (c == '\t') {
	int nspace = 8 - (X(s) & 7);
	if (X(s) + nspace >= COLS) {
	    Newline(w);
	} else {
	    while(nspace-- > 0)
		Add_Char(w, ' ');
	}
    } else if (c == '\r') {
	X(s) = 0;
    } else if (c >= 1 && c <= 26) {
	Add_Char(w,'^');
	Add_Char(w,(c-1) + 'A');
    } else {
	Set_Char(w, Y(s), X(s), c);
	if (X(s) == COLS-1)
	    Newline(w);
	else
	   X(s) = X(s) + 1;
    }
}

/* Invalidate entire window. */
static void Invalidate(WINDOW *w)
{
    struct Screen *s = WORK(w);
    int j;
    for (j = 0; j < LINES; ++j)
	LINE(s,j)->modified = true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

WINDOW *stdscr = &s_stdscr;
int LINES = 25;
int COLS = 80;

int clear(void)
{
    int j;
    for (j = 0; j < LINES; ++j) {
	move(j, 0);
	clrtoeol();
    }
    move(0,0);
    return OK;
}

int getch(void) {
    return Get_Key();
}

int move(int y, int x) {
    struct Screen *s = WORK(stdscr);
    Y(s) = Move_Coord(y, LINES);
    X(s) = Move_Coord(x, COLS);
    return OK;
}

int addch(int c) {
    Add_Char(stdscr, c);
    return OK;
}

int clrtoeol(void) {
    struct Screen *s = WORK(stdscr);
    Set_Char(stdscr, Y(s), X(s), EOL);
    return OK;
}

int clrtobot(void)
{
    struct Screen *s = WORK(stdscr);
    int j;
    clrtoeol();
    for (j = Y(s) + 1; j < LINES; ++j)
	Set_Char(stdscr, j, 0, EOL);
    return OK;
}

int mvaddstr(int y, int x, const char *s)
{
    move(y, x);
    while (*s != '\0')
	addch(*s++);
    return OK;
}

/*
 * Here is the complicated part - take the working screen image
 * and find a way to update the real screen to match,
 * trying to avoid unnecessary output.
 */
int refresh(void)
{
    int i, j;
    struct Screen *work = WORK(stdscr), *display = DISPLAY(stdscr);

    /*
     * Scroll as needed.
     */
    if (stdscr->nScroll > 0) {
	Put_Cursor(LINES-1,0);
	while (stdscr->nScroll-- > 0) {
	    Put_Char('\n');
	    Scroll_Screen(display);
	}
    }

    /*
     * Update each modified line
     */
    for (j = 0; j < LINES; ++j) {
	struct Line *workLine = LINE(WORK(stdscr), j);
	struct Line *displayLine = LINE(DISPLAY(stdscr), j);

	if (workLine->modified) {
	    bool identical = true;
	    for (i = 0; i < COLS; ++i) {
		int ch = workLine->buf[i];

		if(ch == EOL) {
		    displayLine->buf[i] = EOL;
		    break;
		}

		/* Avoid redrawing the identical prefix of the line */
		if (identical) {
		    if (workLine->buf[i] == displayLine->buf[i])
			continue;
		    Put_Cursor(j, i);
		    identical = false;
		}

		displayLine->buf[i] = ch;
		Put_Char(ch);
	    }
	    if (identical)
		Put_Cursor(j, i);
	    Print("\x1B[K");/* Clear to EOL */
	    workLine->modified = false;
	}
    }

    /*
     * Update cursor position.
     */
    Put_Cursor(Y(work), X(work));
    Y(display) = Y(work);
    X(display) = X(work);

    return OK;
}

WINDOW *initscr(void)
{
    clear();
    Invalidate(stdscr);
    return stdscr;
}

int raw(void)
{
    return OK;
}

int noecho(void)
{
    return OK;
}

int idlok(WINDOW *w, bool bf)
{
    return OK;
}

int endwin(void)
{
    move(LINES-1,0);
    clrtoeol();
    refresh();
    return OK;
}
