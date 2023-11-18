/*
 * Symbol mangling macros
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.6 $
 * 
 * The _S macro mangles a symbol name into whatever format is
 * needed for external linkage.  E.g., prepend an underscore
 * for PECOFF.
 */

#ifndef GEEKOS_SYMBOL_H
#define GEEKOS_SYMBOL_H

#ifdef NEED_UNDERSCORE
#  define _S(sym) "_" #sym
#else
#  define _S(sym) #sym
#endif

#endif  /* GEEKOS_SYMBOL_H */
