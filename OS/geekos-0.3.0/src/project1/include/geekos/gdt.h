/*
 * Initialize kernel GDT.
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.7 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_GDT_H
#define GEEKOS_GDT_H

struct Segment_Descriptor;

void Init_GDT(void);
struct Segment_Descriptor* Allocate_Segment_Descriptor(void);
void Free_Segment_Descriptor(struct Segment_Descriptor* desc);
int Get_Descriptor_Index(struct Segment_Descriptor* desc);

#endif  /* GEEKOS_GDT_H */
