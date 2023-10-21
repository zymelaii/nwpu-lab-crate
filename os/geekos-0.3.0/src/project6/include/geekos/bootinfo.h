/*
 * Boot information structure, passed to kernel Main() routine
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.6 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_BOOTINFO_H
#define GEEKOS_BOOTINFO_H

struct Boot_Info {
    int bootInfoSize;	 /* size of this struct; for versioning */
    int memSizeKB;	 /* number of KB, as reported by int 15h */
};

#endif  /* GEEKOS_BOOTINFO_H */
