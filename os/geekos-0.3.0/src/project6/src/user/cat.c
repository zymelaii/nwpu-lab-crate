
/*
 * cat - send files to stdout
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.12 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <conio.h>
#include <process.h>
#include <fileio.h>

int main(int argc, char *argv[])
{
    int i;
    int ret;
    int read;
    int inFd;
    struct VFS_File_Stat stat;
    char buffer[1024];

    if (argc == 1) {
	/* Just copy stdin to stdout */
	do {
	    ret = Read(0, buffer, sizeof(buffer));
	    if (ret < 0) {
		Print("Error: Could not read from stdin: %s\n", Get_Error_String(ret));
		break;
	    }

	    if (ret > 0) {
		Write(1, buffer, ret);
	    }
	} while (ret != 0);

	return !(ret == 0);
    }

    for (i = 1; i < argc; ++i) {
	/* Copy files to stdout */
	const char *filename = argv[i];

	inFd = Open(filename, O_READ);
	if (inFd < 0) {
            Print ("Error: unable to open %s\n", filename);
	    Exit(1);
	}

	ret = FStat(inFd, &stat); 
	if (ret != 0) {
            Print ("Error: could not stat %s: %s\n", filename, Get_Error_String(ret));
	    Exit(1);
	}
	if (stat.isDirectory) {
            Print ("cp can not copy directories\n");
	    Exit(1);
	}

	for (read =0; read < stat.size; read += ret) {
	    int rc;
            ret = Read(inFd, buffer, sizeof(buffer));
	    if (ret < 0) {
		Print("error reading file for copy: %s\n", Get_Error_String(ret));
		Exit(1);
	    }

	    buffer[ret] = '\0';
	    rc = Write(1, buffer, ret);
	    if (rc < 0) {
		Print("Could not write to stdout: %s\n", Get_Error_String(rc));
		Exit(1);
	    }
	}

	Close(inFd);
    }

    return 0;
}
