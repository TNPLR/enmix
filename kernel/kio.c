#include "cpuio.h"
#include "vga.h"
#include "kio.h"
void (*kputc)(char const);
void (*kputs)(char const *);
void set_kputc(void (*inputc)(char const))
{
  kputc = inputc;
}
void set_kputs(void (*inputs)(char const *))
{
  kputs = inputs;
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
