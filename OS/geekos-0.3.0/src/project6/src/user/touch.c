/*
 * touch - Create an empty file
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <fileio.h>
#include <process.h>

static void Print_Error(const char *msg, int err)
{
    Print("%s: %s\n", msg, Get_Error_String(err));
    Exit(1);
}

int main(int argc, char **argv)
{
    int fd;

    if (argc != 2) {
	Print("Usage: touch <filename>\n");
	Exit(1);
    }

    fd = Open(argv[1], O_CREATE|O_WRITE);
    if (fd < 0)
	Print_Error("Could not open file", fd);
    if (Close(fd) < 0)
	Print_Error("Could not close file", fd);

    return 0;
}

