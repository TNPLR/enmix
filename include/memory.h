#ifndef MEMORY_H_
#define MEMORY_H_
#include "bitmap.h"
#include <stdint.h>
// page executive-disable
#define PG_XD_1 0x8000000000000000ULL
#define PG_XD_0 0
// Page present
#define PG_P_1 1
#define PG_P_0 0
// Page access
#define PG_RW_R 0 // Read, execute
#define PG_RW_W 2 // Read, write , execute
#define PG_US_S 0 // System
#define PG_US_U 4 // User
struct virtual_addr {
  struct bitmap vaddr_bitmap;
  uint64_t vaddr_start;
};

struct page_index {
  uint64_t * pml4e;
  uint64_t * pdpte;
  uint64_t * pde;
  uint64_t * pte;
};

enum pool_flags {
  PF_KERNEL = 1,
  PF_USER = 2
};

extern struct pool kernel_pool, user_pool;
void mem_init(void);
void * malloc_page(enum pool_flags pf, uint64_t pg_cnt);
void * get_kernel_pages(uint64_t pg_cnt);
void enable_no_execute(void);
void disable_no_execute(void);
#endif
