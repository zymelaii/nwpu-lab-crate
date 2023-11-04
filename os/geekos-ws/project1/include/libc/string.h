/*
 * String library
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.19 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memset(void* s, int c, size_t n);
void* memcpy(void *dst, const void* src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char* s);
size_t strnlen(const char *s, size_t maxlen);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t limit);
char *strcat(char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t limit);
char *strdup(const char *s1);
int atoi(const char *buf);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strpbrk(const char *s, const char *accept);

/* Note: The ISO C standard puts this in <stdio.h>, but we don't
 * have that header in GeekOS (yet). */
int snprintf(char *s, size_t size, const char *fmt, ...)
    __attribute__ ((__format__ (__printf__, 3, 4)));

#endif  /* STRING_H */
