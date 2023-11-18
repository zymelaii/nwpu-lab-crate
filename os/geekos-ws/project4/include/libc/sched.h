/*
 * Scheduling system calls
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.7 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef SCHED_H
#define SCHED_H

int Set_Scheduling_Policy(int policy, int quantum);
int Get_Time_Of_Day(void);

#endif  /* SCHED_H */

