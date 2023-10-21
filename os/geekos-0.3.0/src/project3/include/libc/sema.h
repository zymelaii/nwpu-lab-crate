/*
 * Semaphores
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.7 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef SEMA_H
#define SEMA_H

int Create_Semaphore(const char *name, int ival);
int P(int sem);
int V(int sem);
int Destroy_Semaphore(int sem);

#endif  /* SEMA_H */

