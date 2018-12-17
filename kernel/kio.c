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
