#include "string.h"
#include <stddef.h>
void *memcpy(void * restrict s1, const void * restrict s2, size_t n)
{
  void *ret = s1;
  for (;n--;) {
    *(char *)(s1++) = *(const char *)(s2++);
  }
  return ret;
}
char *strcpy(char * restrict s1, const char * restrict s2)
{
  char *tmp = s1;
  while (*s2 != '\0') {
    *s1++ = *s2++;
  }
  return tmp;
}
void *memset(void *s, int c, size_t n)
{
  void *tmp = s;
  while (n--) {
    *(unsigned char *)(s++) = (unsigned char)c;
  }
  return tmp;
}
