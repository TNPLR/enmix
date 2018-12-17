#ifndef CPUIO_H_
#define CPUIO_H_
#include <stdint.h>
static inline void poutb(uint16_t port, uint8_t value)
{
  asm volatile("outb %%al, %%dx"::"a"(value), "d"(port));
}
static inline uint8_t pinb(uint16_t port)
{
  uint8_t ret_val;
  asm volatile("inb %%dx, %%al":"=a"(ret_val):"d"(port));
  return ret_val;
}
static inline uint16_t pinw(uint16_t port)
{
  uint16_t ret_val;
  asm volatile("inw %%dx, %%ax":"=a"(ret_val):"d"(port));
  return ret_val;
}
#endif
