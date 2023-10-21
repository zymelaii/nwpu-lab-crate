
/* 
 * Library of user space routines.  Most of these are direct
 *   wrappers for system calls.
 *
 */

#include <stddef.h>

typedef unsigned short Keycode;

int Null(void);
void Exit(void);
int Print_String(const char* message);
Keycode Get_Key(void);
int Spawn_Program(char* program);
int Wait(unsigned int pid);

void *Malloc(unsigned int size);

void* memset(void* s, int c, size_t n);
void* memcpy(void *dst, const void* src, size_t n);
size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
char *strcat(char *s1, char *s2);
char *strdup(char *s1);
char *strcpy(char *s1, char *s2);

