#include "kio.h"
#include "vga.h"
void init_std_put()
{
  set_kputc(vga_putc);
}
int kmain(void)
{
  init_std_put();
  kputs("[INFO] [64 BIT Mode] Kernel in 64 bits mode now\n");
  kputs("[INFO] Welcome to Miros\n");
  while (1);
}
