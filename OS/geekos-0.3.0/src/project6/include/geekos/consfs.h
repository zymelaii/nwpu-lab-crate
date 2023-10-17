/*
 * Console pseudo-filesystem
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_CONSFS_H
#define GEEKOS_CONSFS_H

struct File;

struct File *Open_Console_Input(void);
struct File *Open_Console_Output(void);

#endif /* GEEKOS_CONSFS_H */

