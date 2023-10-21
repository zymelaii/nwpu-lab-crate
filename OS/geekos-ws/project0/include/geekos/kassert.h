/*
 * Definition of KASSERT() macro, and other useful debug macros
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.16 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_KASSERT_H
#define GEEKOS_KASSERT_H

#include <geekos/screen.h>

#ifndef NDEBUG

struct Kernel_Thread;
extern struct Kernel_Thread* g_currentThread;

#define KASSERT(cond) 					\
do {							\
    if (!(cond)) {					\
	Set_Current_Attr(ATTRIB(RED, GRAY|BRIGHT));	\
	Print("Failed assertion in %s: %s at %s, line %d, RA=%lx, thread=%p\n",\
		__func__, #cond, __FILE__, __LINE__,	\
		(ulong_t) __builtin_return_address(0),	\
		g_currentThread);			\
	while (1)					\
	   ; 						\
    }							\
} while (0)

#define TODO(message)					\
do {							\
    Set_Current_Attr(ATTRIB(BLUE, GRAY|BRIGHT));	\
    Print("Unimplemented feature: %s\n", (message));	\
    while (1)						\
	;						\
} while (0)

/*
 * Spin for some number of iterations.
 * This is useful for slowing down things that go by too
 * quickly.
 */
#define PAUSE(count)			\
do {					\
    ulong_t i;				\
    for (i = 0; i < (count); ++i)	\
	;				\
} while (0)

#else

/*
 * The debug macros are no-ops when NDEBUG is defined.
 */
#define KASSERT(cond)
#define TODO(message)
#define PAUSE(count)

#endif

/*
 * Stop dead.
 * Its behavior does not depend on whether or not this
 * is a debug build.
 */
#define STOP() while (1)

/*
 * Panic function.
 */
#define Panic(args...)				\
do {						\
    Set_Current_Attr(ATTRIB(RED, GRAY|BRIGHT));	\
    Print(args);				\
    while (1) ;					\
} while (0)

#endif  /* GEEKOS_KASSERT_H */
