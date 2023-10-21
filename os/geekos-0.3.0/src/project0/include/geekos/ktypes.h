/*
 * Kernel data types
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_KTYPES_H
#define GEEKOS_KTYPES_H

/*
 * GeekOS uses the C99 bool type, with true and false
 * constant values.
 */
#include <stdbool.h>

/*
 * Shorthand for commonly used integer types.
 */
typedef unsigned long ulong_t;
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
typedef unsigned char uchar_t;

/*
 * MIN() and MAX() macros.
 * By using gcc extensions, they are type-correct and
 * evaulate their arguments only once.
 */
#define MIN(a,b) ({typeof (a) _a = (a); typeof (b) _b = (b); (_a < _b) ? _a : _b; })
#define MAX(a,b) ({typeof (a) _a = (a); typeof (b) _b = (b); (_a < _b) ? _a : _b; })

/*
 * Some ASCII character access and manipulation macros.
 */
#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define TOLOWER(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) + ('a' - 'A')) : (c))
#define TOUPPER(c) (((c) >= 'a' && (c) <= 'z') ? ((c) - ('a' - 'A')) : (c))

#endif  /* GEEKOS_KTYPES_H */
