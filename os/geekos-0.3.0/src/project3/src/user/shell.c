/*
 * A really, really simple shell program
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.18 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <conio.h>
#include <process.h>
#include <string.h>

#define BUFSIZE 79
#define DEFAULT_PATH "/c:/a"

#define INFILE	0x1
#define OUTFILE	0x2
#define PIPE	0x4

#define ISSPACE(c) ((c) == ' ' || (c) == '\t')

struct Process {
    int flags;
    char program[BUFSIZE+1];
    char infile[BUFSIZE+1];
    char outfile[BUFSIZE+1];
    char *command;
    int pid;
    int readfd, writefd;
    int pipefd;
};

char *Strip_Leading_Whitespace(char *s);
void Trim_Newline(char *s);
char *Copy_Token(char *token, char *s);
int Build_Pipeline(char *command, struct Process procList[]);
void Spawn_Single_Command(struct Process procList[], int nproc, const char *path);

/* Maximum number of processes allowed in a pipeline. */
#define MAXPROC 5

int exitCodes = 0;

int main(int argc, char **argv)
{
    int nproc;
    char commandBuf[BUFSIZE+1];
    struct Process procList[MAXPROC];
    char path[BUFSIZE+1] = DEFAULT_PATH;
    char *command;

    /* Set attribute to gray on black. */
    Print("\x1B[37m");

    while (true) {
	/* Print shell prompt (bright cyan on black background) */
	Print("\x1B[1;36m$\x1B[37m ");

	/* Read a line of input */
	Read_Line(commandBuf, sizeof(commandBuf));
	command = Strip_Leading_Whitespace(commandBuf);
	Trim_Newline(command);

	/*
	 * Handle some special commands
	 */
	if (strcmp(command, "exit") == 0) {
	    /* Exit the shell */
	    break;
	} else if (strcmp(command, "pid") == 0) {
	    /* Print the pid of this process */
	    Print("%d\n", Get_PID());
	    continue;
	} else if (strcmp(command, "exitCodes") == 0) {
	    /* Print exit codes of spawned processes. */
	    exitCodes = 1;
	    continue;
	} else if (strncmp(command, "path=", 5) == 0) {
	    /* Set the executable search path */
	    strcpy(path, command + 5);
	    continue;
	} else if (strcmp(command, "") == 0) {
	    /* Blank line. */
	    continue;
	}

	/*
	 * Parse the command string and build array of
	 * Process structs representing a pipeline of commands.
	 */
	nproc = Build_Pipeline(command, procList);
	if (nproc <= 0)
	    continue;

	Spawn_Single_Command(procList, nproc, path);
    }

    Print_String("DONE!\n");
    return 0;
}

/*
 * Skip leading whitespace characters in given string.
 * Returns pointer to first non-whitespace character in the string,
 * which may be the end of the string.
 */
char *Strip_Leading_Whitespace(char *s)
{
    while (ISSPACE(*s))
	++s;
    return s;
}

/*
 * Destructively trim newline from string
 * by changing it to a nul character.
 */
void Trim_Newline(char *s)
{
    char *c = strchr(s, '\n');
    if (c != 0)
	*c = '\0';
}

/*
 * Copy a single token from given string.
 * If a token is found, returns pointer to the
 * position in the string immediately past the token:
 * i.e., where parsing for the next token can begin.
 * If no token is found, returns null.
 */
char *Copy_Token(char *token, char *s)
{
    char *t = token;

    while (ISSPACE(*s))
	++s;
    while (*s != '\0' && !ISSPACE(*s))
	*t++ = *s++;
    *t = '\0';

    return *token != '\0' ? s : 0;
}

/*
 * Build process pipeline.
 */
int Build_Pipeline(char *command, struct Process procList[])
{
    int nproc = 0, i;

    while (nproc < MAXPROC) {
        struct Process *proc = &procList[nproc];
        char *p, *s;

        proc->flags = 0;

        command = Strip_Leading_Whitespace(command);
        p = command;

        if (strcmp(p, "") == 0)
	    break;

        ++nproc;

        s = strpbrk(p, "<>|");

        /* Input redirection from file? */
        if (s != 0 && *s == '<') {
	    proc->flags |= INFILE;
	    *s = '\0';
	    p = s+1;
	    s = Copy_Token(proc->infile, p);
	    if (s == 0) {
	        Print("Error: invalid input redirection\n");
	        return -1;
	    }
	    p = s;

	    /* Output redirection still allowed for this command. */
	    p = Strip_Leading_Whitespace(p);
	    s = (*p == '>' || *p == '|') ? p : 0;
        }

        /* Output redirection to file or pipe? */
        if (s != 0 && (*s == '>' || *s == '|')) {
	    bool outfile = (*s == '>');
	    proc->flags |= (outfile ? OUTFILE : PIPE);
	    *s = '\0';
	    p = s+1;
	    if (outfile) {
	        s = Copy_Token(proc->outfile, p);
	        if (s == 0) {
		    Print("Error: invalid output redirection\n");
		    return -1;
	        }
	        p = s;
	    }
        }

        proc->command = command;
        /*Print("command=%s\n", command);*/
        if (!Copy_Token(proc->program, command)) {
	    Print("Error: invalid command\n");
	    return -1;
        }

        if (p == command)
	    command = "";
        else
	    command = p;
    }

    if (strcmp(command,"") != 0) {
	Print("Error: too many commands in pipeline\n");
	return -1;
    }

#if 0
    for (i = 0; i < nproc; ++i) {
        struct Process *proc = &procList[i];
        Print("program=%s, command=\"%s\"\n", proc->program, proc->command);
        if (proc->flags & INFILE)
	    Print("\tinfile=%s\n", proc->infile);
        if (proc->flags & OUTFILE)
	    Print("\toutfile=%s\n", proc->outfile);
        if (proc->flags & PIPE)
	    Print("\tpipe\n");
    }
#endif

    /*
     * Check commands for validity
     */
    for (i = 0; i < nproc; ++i) {
	struct Process *proc = &procList[i];
	if (i > 0 && (proc->flags & INFILE)) {
	    Print("Error: input redirection only allowed for first command\n");
	    return -1;
	}
	if (i < nproc-1 && (proc->flags & OUTFILE)) {
	    Print("Error: output redirection only allowed for last command\n");
	    return -1;
	}
	if (i == nproc-1 && (proc->flags & PIPE)) {
	    Print("Error: unterminated pipeline\n");
	    return -1;
	}
    }

    return nproc;
}

/*
 * Spawn a single command.
 */
void Spawn_Single_Command(struct Process procList[], int nproc, const char *path)
{
    int pid;

    if (nproc > 1) {
	Print("Error: pipes not supported yet\n");
	return;
    }
    if (procList[0].flags & (INFILE|OUTFILE)) {
	Print("Error: I/O redirection not supported yet\n");
	return;
    }

    pid = Spawn_With_Path(procList[0].program, procList[0].command,
	path);
    if (pid < 0)
	Print("Could not spawn process: %s\n", Get_Error_String(pid));
    else {
	int exitCode = Wait(pid);
	if (exitCodes)
	    Print("Exit code was %d\n", exitCode);
    }
}


