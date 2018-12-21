#ifndef MEMORY_H_
#define MEMORY_H_
#include "bitmap.h"
#include <stdint.h>
struct virtual_addr {
  struct bitmap vaddr_bitmap;
  uint64_t vaddr_start;
};
extern struct pool kernel_pool, user_pool;
void mem_init(void);
#endif
