
/*
 * mount - Mount a filesystem on a block device
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.11 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>

int main(int argc, char *argv[])
{
    int ret;

    if (argc != 4) {
        Print("usage: mount <blockdev> <directory> <fstype>\n");
	Exit (1);
    }

    ret = Mount(argv[1], argv[2], argv[3]);
    if (ret != 0) {
        Print("Mount failed: %s\n", Get_Error_String(ret));
	Exit (1);
    }

    return 0;
}
