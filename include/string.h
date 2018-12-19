#ifndef STRING_H_
#define STRING_H_
#include <stddef.h>
void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
char *strcpy(char * restrict s1, const char * restrict s2);
void *memset(void *s, int c, size_t n);
#endif
