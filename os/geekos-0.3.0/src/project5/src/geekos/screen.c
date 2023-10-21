/*
 * GeekOS text screen output
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.24 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <stdarg.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/io.h>
#include <geekos/int.h>
#include <geekos/fmtout.h>
#include <geekos/screen.h>

/*
 * Information sources for VT100 and ANSI escape sequences:
 * - http://www.lns.cornell.edu/~pvhp/dcl/vt100.html
 * - http://en.wikipedia.org/wiki/ANSI_escape_code
 */

/* ----------------------------------------------------------------------
 * Private functions and data
 * ---------------------------------------------------------------------- */

#define ESC ((char) 0x1B)
#define DEFAULT_ATTRIBUTE ATTRIB(BLACK, GRAY)

enum State {
    S_NORMAL,		/* Normal state - output is echoed verbatim */
    S_ESC,		/* Saw ESC character - begin output escape sequence */
    S_ESC2,		/* Saw '[' character - continue output escape sequence */
    S_ARG,		/* Scanning a numeric argument */
    S_CMD,		/* Command */
};

#define MAXARGS 8	/* Max args that can be passed to esc sequence */

struct Console_State {
    /* Current state information */
    int row, col;
    int saveRow, saveCol;
    uchar_t currentAttr;

    /* Working variables for processing escape sequences. */
    enum State state;
    int argList[MAXARGS];
    int numArgs;
};

static struct Console_State s_cons;

#define NUM_SCREEN_DWORDS ((NUMROWS * NUMCOLS * 2) / 4)
#define NUM_SCROLL_DWORDS (((NUMROWS-1) * NUMCOLS * 2) / 4)
#define NUM_DWORDS_PER_LINE ((NUMCOLS*2)/4)
#define FILL_DWORD (0x00200020 | (s_cons.currentAttr<<24) | (s_cons.currentAttr<<8))

/*
 * Scroll the display one line.
 * We speed things up by copying 4 bytes at a time.
 */
static void Scroll(void)
{
    uint_t* v;
    int i, n = NUM_SCROLL_DWORDS;
    uint_t fill = FILL_DWORD;

    /* Move lines 1..NUMROWS-1 up one position. */
    for (v = (uint_t*)VIDMEM, i = 0; i < n; ++i) {
	*v = *(v + NUM_DWORDS_PER_LINE);
	++v;
    }

    /* Clear out last line. */
    for (v = (uint_t*)VIDMEM + n, i = 0; i < NUM_DWORDS_PER_LINE; ++i)
	*v++ = fill;
}

/*
 * Clear current cursor position to end of line using
 * current attribute.
 */
static void Clear_To_EOL(void)
{
    int n = (NUMCOLS - s_cons.col);
    uchar_t* v = VIDMEM + s_cons.row*(NUMCOLS*2) + s_cons.col*2;
    while (n-- > 0) {
	*v++ = ' ';
	*v++ = s_cons.currentAttr;
    }
}

/*
 * Move to the beginning of the next line, scrolling
 * if necessary.
 */
static void Newline(void)
{
    ++s_cons.row;
    s_cons.col = 0;
    if (s_cons.row == NUMROWS) {
	Scroll();
	s_cons.row = NUMROWS - 1;
    }
}

/*
 * Write the graphic representation of given character to the screen
 * at current position, with current attribute, scrolling if
 * necessary.
 */
static void Put_Graphic_Char(int c)
{
    uchar_t* v = VIDMEM + s_cons.row*(NUMCOLS*2) + s_cons.col*2;

    /* Put character at current position */
    *v++ = (uchar_t) c;
    *v = s_cons.currentAttr;

    if (s_cons.col < NUMCOLS - 1)
	++s_cons.col;
    else
	Newline();
}

/*
 * Put one character to the screen using the current cursor position
 * and attribute, scrolling if needed.  The caller should update
 * the cursor position once all characters have been written.
 */
static void Output_Literal_Character(int c)
{
    int numSpaces;

    switch (c) {
    case '\n':
	Clear_To_EOL();
	Newline();
	break;

    case '\t':
	numSpaces = TABWIDTH - (s_cons.col % TABWIDTH);
	while (numSpaces-- > 0)
	    Put_Graphic_Char(' ');
	break;

    default:
	Put_Graphic_Char(c);
	break;
    }

#ifndef NDEBUG
    /*
     * When compiled with --enable-port-e9-hack, Bochs will send writes
     * to port E9 to the console.  This helps tremendously with debugging,
     * because it allows debug Print() statements to be visible after
     * Bochs has exited.
     */
    Out_Byte(0xE9, c);
#endif
}

/*
 * Move the cursor to a new position, stopping at the screen borders.
 */
static void Move_Cursor(int row, int col)
{
    if (row < 0)
	row = 0;
    else if (row >= NUMROWS)
	row = NUMROWS - 1;

    if (col < 0)
	col = 0;
    else if (col >= NUMCOLS)
	col = NUMCOLS - 1;

    s_cons.row = row;
    s_cons.col = col;
}

/*
 * Table mapping ANSI colors to VGA text mode colors.
 */
static const uchar_t s_ansiToVgaColor[] = {
    BLACK,RED,GREEN,AMBER,BLUE,MAGENTA,CYAN,GRAY
};

/*
 * Update the attributes specified by the arguments
 * of the escape sequence.
 */
static void Update_Attributes(void)
{
    int i;
    int attr = s_cons.currentAttr & ~(BRIGHT);

    for (i = 0; i < s_cons.numArgs; ++i) {
	int value = s_cons.argList[i];
	if (value == 0)
	    attr = DEFAULT_ATTRIBUTE;
	else if (value == 1)
	    attr |= BRIGHT;
	else if (value >= 30 && value <= 37)
	    attr = (attr & ~0x7) | s_ansiToVgaColor[value - 30];
	else if (value >= 40 && value <= 47)
	    attr = (attr & ~(0x7 << 4)) | (s_ansiToVgaColor[value - 40] << 4);
    }
    s_cons.currentAttr = attr;
}

/* Reset to cancel or finish processing an escape sequence. */
static void Reset(void)
{
    s_cons.state = S_NORMAL;
    s_cons.numArgs = 0;
}

/* Start an escape sequence. */
static void Start_Escape(void)
{
    s_cons.state = S_ESC;
    s_cons.numArgs = 0;
}

/* Start a numeric argument to an escape sequence. */
static void Start_Arg(int argNum)
{
    KASSERT(s_cons.numArgs == argNum);
    s_cons.numArgs++;
    s_cons.state = S_ARG;
    if (argNum < MAXARGS)
	s_cons.argList[argNum] = 0;
}

/* Save current cursor position. */
static void Save_Cursor(void)
{
    s_cons.saveRow = s_cons.row;
    s_cons.saveCol = s_cons.col;
}

/* Restore saved cursor position. */
static void Restore_Cursor(void)
{
    s_cons.row = s_cons.saveRow;
    s_cons.col = s_cons.saveCol;
}

/* Add a digit to current numeric argument. */
static void Add_Digit(int c)
{
    KASSERT(ISDIGIT(c));
    if (s_cons.numArgs < MAXARGS) {
	int argNum = s_cons.numArgs - 1;
	s_cons.argList[argNum] *= 10;
	s_cons.argList[argNum] += (c - '0');
    }
}

/*
 * Get a numeric argument.
 * Returns zero if that argument was not actually specified.
 */
static int Get_Arg(int argNum)
{
    return argNum < s_cons.numArgs ? s_cons.argList[argNum] : 0;
}

/*
 * The workhorse output function.
 * Depending on the current console output state,
 * does literal character output or processes part of
 * an escape sequence.
 */
static void Put_Char_Imp(int c)
{
again:
    switch (s_cons.state) {
    case S_NORMAL:
	if (c == ESC)
	    Start_Escape();
	else
	    Output_Literal_Character(c);
	break;

    case S_ESC:
	if (c == '[')
	    s_cons.state = S_ESC2;
	else
	    Reset();
	break;

    case S_ESC2:
	if (ISDIGIT(c)) {
	    Start_Arg(0);
	    goto again;
	} else if (c == ';') {
	    /* Special case: for "n;m" commands, "n" is implicitly 1 if omitted */
	    Start_Arg(0);
	    Add_Digit('1');
	    Start_Arg(1);
	} else {
	    s_cons.state = S_CMD;
	    goto again;
	}
	break;

    case S_ARG:
	if (ISDIGIT(c))
	    Add_Digit(c);
	else if (c == ';')
	    Start_Arg(s_cons.numArgs);
	else {
	    s_cons.state = S_CMD;
	    goto again;
	}
	break;

    case S_CMD:
	switch (c) {
	case 'K': Clear_To_EOL(); break;
	case 's': Save_Cursor(); break;
	case 'u': Restore_Cursor(); break;
	case 'A': Move_Cursor(s_cons.row - Get_Arg(0), s_cons.col); break;
	case 'B': Move_Cursor(s_cons.row + Get_Arg(0), s_cons.col); break;
	case 'C': Move_Cursor(s_cons.row, s_cons.col + Get_Arg(0)); break;
	case 'D': Move_Cursor(s_cons.row, s_cons.col - Get_Arg(0)); break;
	case 'm': Update_Attributes(); break;
	case 'f': case 'H':
	    if (s_cons.numArgs == 2) Move_Cursor(Get_Arg(0)-1, Get_Arg(1)-1); break;
	case 'J':
	    if (s_cons.numArgs == 1 && Get_Arg(0) == 2) {
		Clear_Screen();
		Put_Cursor(0, 0);
	    }
	    break;
	default: break;
	}
	Reset();
	break;

    default:
	KASSERT(false);
    }
}

/*
 * Update the location of the hardware cursor.
 */
static void Update_Cursor(void)
{
    /*
     * The cursor location is a character offset from the beginning
     * of page memory (I think).
     */
    uint_t characterPos = (s_cons.row * NUMCOLS) + s_cons.col;
    uchar_t origAddr;

    /*
     * Save original contents of CRT address register.
     * It is considered good programming practice to restore
     * it to its original value after modifying it.
     */
    origAddr = In_Byte(CRT_ADDR_REG);
    IO_Delay();

    /* Set the high cursor location byte */
    Out_Byte(CRT_ADDR_REG, CRT_CURSOR_LOC_HIGH_REG);
    IO_Delay();
    Out_Byte(CRT_DATA_REG, (characterPos>>8) & 0xff);
    IO_Delay();

    /* Set the low cursor location byte */
    Out_Byte(CRT_ADDR_REG, CRT_CURSOR_LOC_LOW_REG);
    IO_Delay();
    Out_Byte(CRT_DATA_REG, characterPos & 0xff);
    IO_Delay();

    /* Restore contents of the CRT address register */
    Out_Byte(CRT_ADDR_REG, origAddr);
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize the screen module.
 */
void Init_Screen(void)
{
    bool iflag = Begin_Int_Atomic();

    s_cons.row = s_cons.col = 0;
    s_cons.currentAttr = DEFAULT_ATTRIBUTE;
    Clear_Screen();

    End_Int_Atomic(iflag);
}

/*
 * Clear the screen using the current attribute.
 */
void Clear_Screen(void)
{
    uint_t* v = (uint_t*)VIDMEM;
    int i;
    uint_t fill = FILL_DWORD;

    bool iflag = Begin_Int_Atomic();

    for (i = 0; i < NUM_SCREEN_DWORDS; ++i)
	*v++ = fill;

    End_Int_Atomic(iflag);
}

/*
 * Get current cursor position.
 */
void Get_Cursor(int* row, int* col)
{
    bool iflag = Begin_Int_Atomic();
    *row = s_cons.row;
    *col = s_cons.col;
    End_Int_Atomic(iflag);
}

/*
 * Set the current cursor position.
 * Return true if successful, or false if the specified
 * cursor position is invalid.
 */
bool Put_Cursor(int row, int col)
{
    bool iflag;

    if (row < 0 || row >= NUMROWS || col < 0 || col >= NUMCOLS)
	return false;

    iflag = Begin_Int_Atomic();
    s_cons.row = row;
    s_cons.col = col;
    Update_Cursor();
    End_Int_Atomic(iflag);

    return true;
}

/*
 * Get the current character attribute.
 */
uchar_t Get_Current_Attr(void)
{
    return s_cons.currentAttr;
}

/*
 * Set the current character attribute.
 */
void Set_Current_Attr(uchar_t attrib)
{
    bool iflag = Begin_Int_Atomic();
    s_cons.currentAttr = attrib;
    End_Int_Atomic(iflag);
}

/*
 * Write a single character to the screen at current position
 * using current attribute, handling scrolling, special characters, etc.
 */
void Put_Char(int c)
{
    bool iflag = Begin_Int_Atomic();
    Put_Char_Imp(c);
    Update_Cursor();
    End_Int_Atomic(iflag);
}

/*
 * Write a string of characters to the screen at current cursor
 * position using current attribute.
 */
void Put_String(const char* s)
{
    bool iflag = Begin_Int_Atomic();
    while (*s != '\0')
	Put_Char_Imp(*s++);
    Update_Cursor();
    End_Int_Atomic(iflag);
}

/*
 * Write a buffer of characters at current cursor position
 * using current attribute.
 */
void Put_Buf(const char* buf, ulong_t length)
{
    bool iflag = Begin_Int_Atomic();
    while (length > 0) {
	Put_Char_Imp(*buf++);
	--length;
    }
    Update_Cursor();
    End_Int_Atomic(iflag);
}

/* Support for Print(). */
static void Print_Emit(struct Output_Sink *o, int ch) { Put_Char_Imp(ch); }
static void Print_Finish(struct Output_Sink *o) { Update_Cursor(); }
static struct Output_Sink s_outputSink = { &Print_Emit, &Print_Finish };

/*
 * Print to console using printf()-style formatting.
 * Calls into Format_Output in common library.
 */
void Print(const char *fmt, ...)
{
    va_list args;

    bool iflag = Begin_Int_Atomic();

    va_start(args, fmt);
    Format_Output(&s_outputSink, fmt, args);
    va_end(args);

    End_Int_Atomic(iflag);
}

