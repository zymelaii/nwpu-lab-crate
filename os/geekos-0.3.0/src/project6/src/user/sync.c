/*
 * sync - Flush all cached filesystem data to disk
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>

int main(int argc, char **argv)
{
    int rc;

    rc = Sync();
    if (rc != 0)
	Print("Could not sync filesystems: %s\n", Get_Error_String(rc));

    return !(rc == 0);
}

