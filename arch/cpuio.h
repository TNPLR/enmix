#ifndef CPUIO_H_
#define CPUIO_H_
#include <stdint.h>
#if __x86_64__ || __i386__
static inline __attribute__((always_inline)) void outb(uint8_t, uint16_t);
static inline __attribute__((always_inline)) void outw(uint16_t, uint16_t);
static inline __attribute__((always_inline)) void outl(uint32_t, uint16_t);
static inline __attribute__((always_inline)) uint8_t inb(uint16_t);
static inline __attribute__((always_inline)) uint16_t inw(uint16_t);
static inline __attribute__((always_inline)) uint32_t inl(uint16_t);

static inline void outb(uint8_t val, uint16_t port)
{
  asm volatile("outb %0, %1"::"a"(val), "d"(port));
}

static inline void outw(uint16_t val, uint16_t port)
{
  asm volatile("outw %0, %1"::"a"(val), "d"(port));
}

static inline void outl(uint32_t val, uint16_t port)
{
  asm volatile("outl %0, %1"::"a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port)
{
  uint8_t ret;
  asm volatile("inb %1, %0":"=a"(ret):"d"(port));
  return ret;
}

static inline uint16_t inw(uint16_t port)
{
  uint16_t ret;
  asm volatile("inw %1, %0":"=a"(ret):"d"(port));
  return ret;
}

static inline uint32_t inl(uint16_t port)
{
  uint32_t ret;
  asm volatile("inl %1, %0":"=a"(ret):"d"(port));
  return ret;
}
#if __x86_64__
static inline __attribute__((always_inline)) void outq(uint64_t, uint16_t);
static inline __attribute__((always_inline)) uint64_t inq(uint16_t);

static inline void outq(uint64_t val, uint16_t port)
{
  asm volatile("outq %0, %1"::"a"(val), "d"(port));
}

static inline uint64_t inq(uint16_t port)
{
  uint64_t ret;
  asm volatile("inq %1, %0":"=a"(ret):"d"(port));
  return ret;
}
#endif /* __x86_64__ */
#else
_Static_assert(0, "No Architecture is Defined");
#endif /* __x86_64__ || __i386__ */
#endif /* CPUIO_H_ */
