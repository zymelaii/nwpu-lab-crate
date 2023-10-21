/*
 * String library
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.21 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

/*
 * NOTE:
 * These are slow and simple implementations of a subset of
 * the standard C library string functions.
 * We also have an implementation of snprintf().
 */

#include <fmtout.h>
#include <string.h>

extern void *Malloc(size_t size);

void* memset(void* s, int c, size_t n)
{
    unsigned char* p = (unsigned char*) s;

    while (n > 0) {
	*p++ = (unsigned char) c;
	--n;
    }

    return s;
}

void* memcpy(void *dst, const void* src, size_t n)
{
    unsigned char* d = (unsigned char*) dst;
    const unsigned char* s = (const unsigned char*) src;

    while (n > 0) {
	*d++ = *s++;
	--n;
    }

    return dst;
}

int memcmp(const void *s1_, const void *s2_, size_t n)
{
    const signed char *s1 = s1_, *s2 = s2_;

    while (n > 0) {
	int cmp = *s1 - *s2;
	if (cmp != 0)
	    return cmp;
	++s1;
	++s2;
    }

    return 0;
}

size_t strlen(const char* s)
{
    size_t len = 0;
    while (*s++ != '\0')
	++len;
    return len;
}

/*
 * This it a GNU extension.
 * It is like strlen(), but it will check at most maxlen
 * characters for the terminating nul character,
 * returning maxlen if it doesn't find a nul.
 * This is very useful for checking the length of untrusted
 * strings (e.g., from user space).
 */
size_t strnlen(const char *s, size_t maxlen)
{
    size_t len = 0;
    while (len < maxlen && *s++ != '\0')
	++len;
    return len;
}

int strcmp(const char* s1, const char* s2)
{
    while (1) {
	int cmp = *s1 - *s2;
	if (cmp != 0 || *s1 == '\0' || *s2 == '\0')
	    return cmp;
	++s1;
	++s2;
    }
}

int strncmp(const char* s1, const char* s2, size_t limit)
{
    size_t i = 0;
    while (i < limit) {
	int cmp = *s1 - *s2;
	if (cmp != 0 || *s1 == '\0' || *s2 == '\0')
	    return cmp;
	++s1;
	++s2;
	++i;
    }

    /* limit reached and equal */
    return 0;
}

char *strcat(char *s1, const char *s2)
{
    char *t1;

    t1 = s1;
    while (*s1) s1++;
    while(*s2) *s1++ = *s2++;
    *s1 = '\0';

    return t1;
}

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';

    return ret;
}

char *strncpy(char *dest, const char *src, size_t limit)
{
    char *ret = dest;

    while (*src != '\0' && limit > 0) {
	*dest++ = *src++;
	--limit;
    }
    if (limit > 0)
	*dest = '\0';

    return ret;
}

char *strdup(const char *s1)
{
    char *ret;

    ret = Malloc(strlen(s1) + 1);
    strcpy(ret, s1);

    return ret;
}

int atoi(const char *buf) 
{
    int ret = 0;

    while (*buf >= '0' && *buf <= '9') {
       ret *= 10;
       ret += *buf - '0';
       buf++;
    }

    return ret;
}

char *strchr(const char *s, int c)
{
    while (*s != '\0') {
	if (*s == c)
	    return (char *) s;
	++s;
    }
    return 0;
}

char *strrchr(const char *s, int c)
{
    size_t len = strlen(s);
    const char *p = s + len;

    while (p > s) {
	--p;
	if (*p == c)
	    return (char*) p;
    }
    return 0;
}

char *strpbrk(const char *s, const char *accept)
{
    size_t setLen = strlen(accept);

    while (*s != '\0') {
	size_t i;
	for (i = 0; i < setLen; ++i) {
	    if (*s == accept[i])
		return (char *) s;
	}
	++s;
    }

    return 0;
}

struct String_Output_Sink {
    struct Output_Sink o;
    char *s;
    size_t n, size;
};

static void String_Emit(struct Output_Sink *o_, int ch)
{
    struct String_Output_Sink *o = (struct String_Output_Sink*) o_;

    if (o->n < o->size)
	*(o->s)++ = ch;
    ++(o->n);
}

static void String_Finish(struct Output_Sink *o_)
{
    struct String_Output_Sink *o = (struct String_Output_Sink*) o_;

    if (o->n < o->size)
	*(o->s) = '\0';
    else
	/*
	 * Output was truncated; write terminator at end of buffer
	 * (we will have advanced one character too far)
	 */
	*(o->s - 1) = '\0';
}

int snprintf(char *s, size_t size, const char *fmt, ...)
{
    struct String_Output_Sink sink;
    int rc;
    va_list args;

    /* Prepare string output sink */
    sink.o.Emit = &String_Emit;
    sink.o.Finish = &String_Finish;
    sink.s = s;
    sink.n = 0;
    sink.size = size;

    /* Format the string */
    va_start(args, fmt);
    rc = Format_Output(&sink.o, fmt, args);
    va_end(args);

    return rc;
}
