/*
 * Process creation and management
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.17 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <stddef.h>
#include <geekos/ktypes.h>
#include <geekos/syscall.h>
#include <geekos/errno.h>
#include <string.h>
#include <process.h>

/* System call wrappers */
DEF_SYSCALL(Null,SYS_NULL,int,(void),,SYSCALL_REGS_0)
DEF_SYSCALL(Exit,SYS_EXIT,int,(int exitCode), int arg0 = exitCode;, SYSCALL_REGS_1)
DEF_SYSCALL(Spawn_Program,SYS_SPAWN,int,
    (const char *program, const char *command),
    const char *arg0 = program; size_t arg1 = strlen(program); const char *arg2 = command; size_t arg3 = strlen(command);,
    SYSCALL_REGS_4)
DEF_SYSCALL(Wait,SYS_WAIT,int,(int pid),int arg0 = pid;,SYSCALL_REGS_1)
DEF_SYSCALL(Get_PID,SYS_GETPID,int,(void),,SYSCALL_REGS_0)

#define CMDLEN 79

static bool Ends_With(const char *name, const char *suffix)
{
    size_t nameLen = strlen(name);
    size_t suffixLen = strlen(suffix);
    size_t start, i;

    if (suffixLen > nameLen)
	return false;
    start = nameLen - suffixLen;

    for (i = 0; i < suffixLen; ++i) {
	if (name[start + i] != suffix[i])
	    return false;
    }
    return true;
}

int Spawn_With_Path(const char *program, const char *command,
    const char *path)
{
    int pid;
    char exeName[(CMDLEN*2)+5];

    /* Try executing program as specified */
    pid = Spawn_Program(program, command
	);

    if (pid == ENOTFOUND && strchr(program, '/') == 0) {
	/* Search for program on path. */
	for (;;) {
	    char *p;

	    while (*path == ':')
		++path;

	    if (strcmp(path, "") == 0)
		break;

	    p = strchr(path, ':');
	    if (p != 0) {
		memcpy(exeName, path, p - path);
		exeName[p - path] = '\0';
		path = p + 1;
	    } else {
		strcpy(exeName, path);
		path = "";
	    }

	    strcat(exeName, "/");
	    strcat(exeName, program);

	    if (!Ends_With(exeName, ".exe"))
		strcat(exeName, ".exe");

	    /*Print("exeName=%s\n", exeName);*/
	    pid = Spawn_Program(exeName, command
		);
	    if (pid != ENOTFOUND)
		break;
	}
    }

    return pid;
}

