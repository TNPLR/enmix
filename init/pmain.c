#include <arch/cpuio.h>
#include "direct_vga.h"
void pmain(void * boot_info) __attribute__((section(".text32")));

void pmain(void * boot_info)
{
  dvga_putchar('O');
  dvga_putchar('K');
  while (1);
}
