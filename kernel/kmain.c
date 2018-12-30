#include "kio.h"
#include "tty.h"
#include "timer8253.h"
#include "pic.h"
#include "vga.h"
#include "interrupt.h"
#include "e820.h"
#include "memory.h"
#include "thread.h"
#include "assert.h"
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
  set_kputc(tty_putc);
  set_kputs(tty_puts);
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
  disable_interrupt();
  init_std_put();
  tty_init();
  kputs("[INFO] Init start\n");
  kputs("[INFO] STDOUT init\n");
  kputs("[INFO] TTY init\n");
  init_data_segment();
  pic_init();
  idt_init();
  timer8253_init();
  print_ram();
  mem_init();
  thread_sys_init();
  kputs("[INFO] Init done\n");
}

void k_thread_a(void * args)
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
  thread_start("kthread", 15, k_thread_a, "FIRS ");
  //thread_start("kthr", 31, k_thread_a, "SECO ");
  kputs("[DEBUG] Thread start done\n");
  enable_interrupt();
  while (1) {
    //kputs("MAIN ");
  }
  return 0;
}
