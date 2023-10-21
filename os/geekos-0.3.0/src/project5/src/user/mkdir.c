/*
 * mkdir - Create a directory
 * Copyright (c) 2003 Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.9 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>

int main(int argc, char *argv[])
{
    int rc;

    if (argc != 2) {
        Print("Usage: mkdir <directory>\n");
	Exit(1);
    }

    rc = Create_Directory(argv[1]);
    if (rc != 0)
	Print("Could not create directory: %s\n", Get_Error_String(rc));

    return !(rc == 0);
}
