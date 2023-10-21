/*
 * format - Format a filesystem on a block device
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.10 $
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

    if (argc != 3) {
         Print("Usage: format <devname> <fstype>\n");
	 Exit(1);
    }

    rc = Format(argv[1], argv[2]);
    if (rc != 0) {
	Print("Error: Could not format %s as %s: %s\n", argv[1], argv[2], Get_Error_String(rc));
	Exit(1);
    }

    return 0;
}
