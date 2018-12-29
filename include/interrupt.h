#ifndef INTERRUPT_H_
#define INTERRUPT_H_
#include <stdint.h>
void idt_init(void);
void setup_handler(uint8_t, void (*)(uint64_t, uint64_t));
static inline void enable_interrupt(void)
{
  asm volatile("sti");
}

static inline void disable_interrupt(void)
{
  asm volatile("cli");
}

static inline int get_interrupt(void)
{
  uint64_t ret;
  asm volatile("pushfq; popq %%rax":"=a"(ret));
  return (int)ret & 0x200;
}
#endif
