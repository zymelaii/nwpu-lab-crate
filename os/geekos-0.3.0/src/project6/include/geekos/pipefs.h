/*
 * Pipe pseudo-filesystem
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.3 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_PIPEFS_H
#define GEEKOS_PIPEFS_H

struct File;

int Create_Pipe(struct File **pRead, struct File **pWrite);

#endif /* GEEKOS_PIPEFS_H */

