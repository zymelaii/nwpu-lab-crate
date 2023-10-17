/*
 * Process creation and management
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.14 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef PROCESS_H
#define PROCESS_H

int Null(void);
int Exit(int exitCode);
int Spawn_Program(const char* program, const char* command);
int Spawn_With_Path(const char *program, const char *command, const char *path);
int Wait(int pid);
int Get_PID(void);

#endif  /* PROCESS_H */

