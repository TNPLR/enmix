#include "string.h"
#include <stddef.h>
void *memcpy(void * restrict s1, const void * restrict s2, size_t n)
{
  unsigned char * tmp1 = (unsigned char *)s1;
  const unsigned char * tmp2 = (const unsigned char *)s2;
  while (n--) {
    *tmp1++ = *tmp2++;
  }
  return s1;
}
char *strcpy(char * restrict s1, const char * restrict s2)
{
  char *tmp = s1;
  while (*s2 != '\0') {
    *s1++ = *s2++;
  }
  return tmp;
}
void *memset(void * s, int c, size_t n)
{
  unsigned char * tmp = (unsigned char *)s;
  while (n--) {
    *(tmp++) = (unsigned char)c;
  }
  return s;
}
