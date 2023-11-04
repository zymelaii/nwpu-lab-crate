/*
 * User-mode Console I/O
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.25 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/syscall.h>
#include <fmtout.h>
#include <string.h>
#include <conio.h>

static bool s_echo = true;

/* System call wrappers. */
DEF_SYSCALL(Print_String,SYS_PRINTSTRING,int,(const char *str),
    const char *arg0 = str; size_t arg1 = strlen(str);,SYSCALL_REGS_2)
DEF_SYSCALL(Get_Key,SYS_GETKEY,Keycode,(void),,SYSCALL_REGS_0)
DEF_SYSCALL(Set_Attr,SYS_SETATTR,int,(int attr),int arg0 = attr;,SYSCALL_REGS_1)
DEF_SYSCALL(Get_Cursor,SYS_GETCURSOR,int,(int *row, int *col),
    int *arg0 = row; int *arg1 = col;,SYSCALL_REGS_2)


int Put_Cursor(int row, int col)
{
    char command[40];
    /*
     * Note: cursor coordinates in VT100 and ANSI escape sequences
     * are 1-based, not 0-based
     */
    snprintf(command, sizeof(command), "\x1B[%d;%df", row+1, col+1);
    Print_String(command);
    return 0;
}

int Put_Char(int ch)
{
    char buf[2];
    buf[0] = (char) ch;
    buf[1] = '\0';
    return Print_String(buf);
}

void Echo(bool enable)
{
    s_echo = enable;
}

void Read_Line(char* buf, size_t bufSize)
{
    char *ptr = buf;
    size_t n = 0;
    Keycode k;
    bool done = false;
    int startrow = 0, startcol = 0;
    Get_Cursor(&startrow, &startcol);
    /*Print("Start column is %d\n", startcol); */

    bufSize--;
    do {
	k = Get_Key();
	if ((k & KEY_SPECIAL_FLAG) || (k & KEY_RELEASE_FLAG))
	    continue;

	k &= 0xff;
	if (k == '\r')
	    k = '\n';

	if (k == ASCII_BS) {
	    if (n > 0) {
		char last = *(ptr - 1);
		int newcol = startcol;
		size_t i;

		/* Back up in line buffer */
		--ptr;
		--n;

		if (s_echo) {
		    /*
		     * Figure out what the column position of the last
		     * character was
		     */
		    for (i = 0; i < n; ++i) {
			char ch = buf[i];
			if (ch == '\t') {
			    int rem = newcol % TABWIDTH;
			    newcol += (rem == 0) ? TABWIDTH : (TABWIDTH - rem);
			} else {
			    ++newcol;
			}
		    }

		    /* Erase last character */
		    if (last != '\t')
			last = ' ';
		    Put_Cursor(startrow, newcol);
		    Put_Char(last);
		    Put_Cursor(startrow, newcol);
		}
	    }
	    continue;
	}

	if (s_echo)
	    Put_Char(k);

	if (k == '\n')
	    done = true;

	if (n < bufSize) {
	    *ptr++ = k;
	    ++n;
	}
    }
    while (!done);

    *ptr = '\0';
}

const char *Get_Error_String(int errno)
{
    extern const char *__strerrTable[];
    extern const int __strerrTableSize;

    /*
     * Error numbers, as returned by system calls, are
     * always <= 0.  The table of error strings is
     * indexed by the negation of the error code, which
     * is a nonnegative number.
     */
    errno = -errno;

    if (errno < 0 || errno >= __strerrTableSize)
	return "Unknown error";
    else
	return __strerrTable[errno];
}

/* Support for Print(). */
static void Print_Emit(struct Output_Sink *o, int ch) { Put_Char(ch); }
static void Print_Finish(struct Output_Sink *o) { }
static struct Output_Sink s_outputSink = { &Print_Emit, &Print_Finish };

void Print(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    Format_Output(&s_outputSink, fmt, args);
    va_end(args);
}

