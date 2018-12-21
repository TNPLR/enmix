#include "memory.h"
#include "kio.h"
#include "e820.h"
#include <stdint.h>

#define PG_SIZE 0x1000
#define MEM_BITMAP_BASE 0xc0204000
#define K_HEAP_START 0xc0400000

struct pool {
  struct bitmap pool_bitmap;
  uint64_t phy_addr_start;
  uint64_t pool_size;
};

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint64_t all_mem)
{
  kputs("[INFO] Mem_pool init start\n");
  uint64_t used_mem = 0x500000; // low 1mb + kernel 4mb
  uint64_t free_mem = all_mem - used_mem;
  uint64_t all_free_pages = free_mem / PG_SIZE;
  uint64_t kernel_free_pages = all_free_pages / 2;
  uint64_t user_free_pages = all_free_pages - kernel_free_pages;

  uint64_t kbm_length = kernel_free_pages / 8; // kernel bitmap length
  uint64_t ubm_length = user_free_pages / 8; // user bitmap length
  // This may make some memory be unused
  uint64_t kp_start = used_mem;
  uint64_t up_start = kp_start + kernel_free_pages * PG_SIZE;
  kernel_pool.phy_addr_start = kp_start;
  user_pool.phy_addr_start = up_start;

  kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
  user_pool.pool_size = user_free_pages * PG_SIZE;

  kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;
  user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

  kernel_pool.pool_bitmap.bits = (void *)MEM_BITMAP_BASE;

  user_pool.pool_bitmap.bits = (void *)(MEM_BITMAP_BASE + kbm_length);

  kputs("[INFO] Kernel pool bitmap start: ");
  kputuint((uint64_t)kernel_pool.pool_bitmap.bits, 16);
  kputs("\n[INFO] Kernel pool physical address start: ");
  kputuint(kernel_pool.phy_addr_start, 16);
  kputs("\n[INFO] User pool bitmap start: ");
  kputuint((uint64_t)user_pool.pool_bitmap.bits, 16);
  kputs("\n[INFO] User pool physical address start: ");
  kputuint(user_pool.phy_addr_start, 16);
  kputs("\n");
  bitmap_init(&kernel_pool.pool_bitmap);
  bitmap_init(&user_pool.pool_bitmap);

  kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;

  kernel_vaddr.vaddr_bitmap.bits =
    (void *)(MEM_BITMAP_BASE + kbm_length + ubm_length); 
  kernel_vaddr.vaddr_start = K_HEAP_START;
  bitmap_init(&kernel_vaddr.vaddr_bitmap);
  kputs("[INFO] Mem pool init done\n");
}

void mem_init(void)
{
  kputs("[INFO] Mem init start\n");
  mem_pool_init(get_ram());
  kputs("[INFO] Mem init done\n");
}
