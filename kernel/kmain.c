#include "kio.h"
#include "timer8253.h"
#include "pic.h"
#include "vga.h"
#include "interrupt.h"
#include "e820.h"
#include "memory.h"
#include "thread.h"
void init_data_segment()
{
  asm volatile("movw %0, %%ax;"
      "movw %%ax, %%ds;"
      "movw %%ax, %%es;"
      "movw %%ax, %%fs;"
      "movw %%ax, %%gs;"
      "movw %%ax, %%ss;"::"i"(0x0):"ax");
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

void k_thread_s(void * args)
{
  const char * str = (const char *)args;
  while (1) {
    kputs(str);
  }
}

int kmain(void)
{
  init_all();
  kputs("[INFO] [64 BIT Mode] Kernel in 64 bits mode now\n");
  kputs("[INFO] Welcome to Miros\n");

  void * addr = get_kernel_pages(3);
  kputs("[INFO] Get kernel pages start vaddr is ");
  kputuint((uint64_t)addr, 16);
  kputs("\n");
  void * addr2 = get_kernel_pages(7);
  kputs("[INFO] Get kernel pages start vaddr is ");
  kputuint((uint64_t)addr2, 16);
  kputs("\n");
  kputuint(sizeof(struct task_struct), 10);
  kputs("\n");
  //thread_start("kthread", 31, k_thread_s, "Thread test\n");
  while (1);
  return 0;
}
