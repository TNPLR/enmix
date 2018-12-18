#include "cpuio.h"
#include "vga.h"
#include "kio.h"
void (*kputc)(char const);
void set_kputc(void (*inputc)(char const))
{
  kputc = inputc;
}
void kputs(char const *s)
{
  while(*s) {
    kputc(*s);
    ++s;
  }
}

void kputuint(uintmax_t num, unsigned base)
{
  static char const *digit = "0123456789ABCDEF";
  static char buffer[50];
  char *ptr;
  ptr = &buffer[49];
  *ptr = '\0';
  do {
    *--ptr = digit[num % base];
    num /= base;
  } while (num != 0);
  kputs(ptr);
}
