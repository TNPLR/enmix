#include "kio.h"
#include "timer8253.h"
#include "pic.h"
#include "vga.h"
#include "interrupt.h"
#include "e820.h"
#include "memory.h"
void init_data_segment()
{
  asm volatile("movw %0, %%ax;"
      "movw %%ax, %%ds;"
      "movw %%ax, %%es;"
      "movw %%ax, %%fs;"
      "movw %%ax, %%gs;"
      "movw %%ax, %%ss;"::"i"(0x28):"ax");
}
void init_std_put()
{
  set_kputc(vga_putc);
  kputs("[INFO] Putc set\n");
}
void print_ram(void)
{
  uint64_t mem = get_ram();
  kputs("[INFO] RAM size: ");
  kputuint(convert_ram_gib(mem), 10);
  kputs(" GiB + ");
  kputuint(convert_ram_mib(mem) % 1024, 10);
  kputs(" MiB + ");
  kputuint(convert_ram_kib(mem) % 1024, 10);
  kputs(" KiB + ");
  kputuint(mem % 1024, 10);
  kputs(" Bytes\n");
}
void init_all(void)
{
  init_std_put();
  kputs("[INFO] Init start\n");
  init_data_segment();
  pic_init();
  idt_init();
  timer8253_init();
  enable_interrupt();
  //disable_interrupt();
  print_ram();
  mem_init();
  kputs("[INFO] Init done\n");
}
int kmain(void)
{
  init_all();
  kputs("[INFO] [64 BIT Mode] Kernel in 64 bits mode now\n");
  kputs("[INFO] Welcome to Miros\n");
  while (1);
}
