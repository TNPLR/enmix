#include "memory.h"
#include "kio.h"
#include "e820.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>

#define PG_SIZE 0x1000
#define MEM_BITMAP_BASE 0xc0204000
#define K_HEAP_START 0xc0400000
#define PML4E_IDX(addr) (addr >> 39 & 0x1FF)
#define PDPTE_IDX(addr) (addr >> 30 & 0x1FF)
#define PDE_IDX(addr) (addr >> 21 & 0x1FF)
#define PTE_IDX(addr) (addr >> 12 & 0x1FF)

struct pool {
  struct bitmap pool_bitmap;
  uint64_t phy_addr_start;
  uint64_t pool_size;
};

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;

static void * vaddr_get(enum pool_flags pf, uint64_t pg_cnt)
{
  int64_t vaddr_start = 0, bit_idx_start = -1;
  uint64_t cnt = 0;
  if (pf == PF_KERNEL) {
    bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
    if (bit_idx_start == -1) {
      return NULL;
    }
    while (cnt < pg_cnt) {
      bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt++, 1);
    }
    vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
  } else {
    // TODO: User Memory
  }
  return (void *)vaddr_start;
}

uint64_t * pml4e_ptr(uint64_t vaddr)
{
  uint64_t * pml4e =
    (uint64_t *)((0xFFFFFFFFFFFFF000ULL) + PML4E_IDX(vaddr) * 8);
  return pml4e;
}

uint64_t * pdpte_ptr(uint64_t vaddr)
{
  uint64_t * pdpte =
    (uint64_t *)((0xFFFFFFFFFFE00000ULL) + 
        ((vaddr & 0xFF8000000000ULL) >> 27) + PDPTE_IDX(vaddr) * 8);
  return pdpte;
}

uint64_t * pde_ptr(uint64_t vaddr)
{
  uint64_t * pde =
    (uint64_t *)((0xFFFFFFFFC0000000ULL) + 
        ((vaddr & 0xFF8000000000ULL) >> 18) +
        ((vaddr & 0x7FC0000000ULL) >> 18) + PDE_IDX(vaddr) * 8);
  return pde;
}

uint64_t * pte_ptr(uint64_t vaddr)
{
  uint64_t * pte =
    (uint64_t *)((0xFFFFFF8000000000ULL) + 
        ((vaddr & 0xFF8000000000ULL) >> 9) +
        ((vaddr & 0x7FC0000000ULL) >> 9) +
        ((vaddr & 0x3FE00000ULL) >> 9) + PTE_IDX(vaddr) * 8);
  return pte;
}

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

static void * palloc(struct pool * m_pool)
{
  int bit_idx = bitmap_scan(&m_pool->pool_bitmap, 1);
  if (bit_idx == -1) {
    return NULL;
  }
  bitmap_set(&m_pool->pool_bitmap, bit_idx, 1);
  uint64_t page_phyaddr =
    ((bit_idx * PG_SIZE) + m_pool->phy_addr_start);
  return (void *)page_phyaddr;
}

static void pte_table_add(struct page_index pindex, uint64_t page_phyaddr)
{
  if (!(*pindex.pte & 0x1)) {
    *pindex.pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
  } else {
    kputs("[WARNIN] PTE repeat\n");
    *pindex.pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
  }
}

static void pde_table_add(struct page_index pindex, uint64_t page_phyaddr)
{
  if (*pindex.pde & 0x1) {
    pte_table_add(pindex, page_phyaddr);
  } else {
    // make a pde
    uint64_t pde_phyaddr = (uint64_t)palloc(&kernel_pool);
    *pindex.pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    pte_table_add(pindex, page_phyaddr);
  }
}

static void pdpte_table_add(struct page_index pindex, uint64_t page_phyaddr)
{
  if (*pindex.pdpte & 0x1) {
    pde_table_add(pindex, page_phyaddr);
  } else {
    uint64_t pdpte_phyaddr = (uint64_t)palloc(&kernel_pool);
    *pindex.pdpte = (pdpte_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    pde_table_add(pindex, page_phyaddr);
  }
}

static void pml4e_table_add(struct page_index pindex, uint64_t page_phyaddr)
{
  if (*pindex.pml4e & 0x1) {
    pdpte_table_add(pindex, page_phyaddr);
  } else {
    // Add a new pdpte
    uint64_t pml4e_phyaddr = (uint64_t)palloc(&kernel_pool);
    *pindex.pml4e = (pml4e_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    pdpte_table_add(pindex, page_phyaddr);
  }
}
static void page_table_add(void * i_vaddr, void * i_page_phyaddr)
{
  uint64_t vaddr = (uint64_t)i_vaddr;
  uint64_t page_phyaddr = (uint64_t)i_page_phyaddr;

  struct page_index pindex;
  pindex.pml4e = pml4e_ptr(vaddr);
  pindex.pdpte = pdpte_ptr(vaddr);
  pindex.pde = pde_ptr(vaddr);
  pindex.pte = pte_ptr(vaddr);

  pml4e_table_add(pindex, page_phyaddr);
}

void * malloc_page(enum pool_flags pf, uint64_t pg_cnt)
{
  void * vaddr_start = vaddr_get(pf, pg_cnt);
  if (vaddr_start == NULL) {
    return NULL;
  }
  uint64_t vaddr = (uint64_t)vaddr_start;
  uint64_t cnt = pg_cnt;
  struct pool * mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;

  while (cnt-- > 0) {
    void * page_phyaddr = palloc(mem_pool);
    if (page_phyaddr == NULL) {
      return NULL;
    }
    page_table_add((void *)vaddr, page_phyaddr);
    vaddr += PG_SIZE;
  }
  return vaddr_start;
}

void * get_kernel_pages(uint64_t pg_cnt)
{
  void * vaddr = malloc_page(PF_KERNEL, pg_cnt);
  if (vaddr != NULL) {
    memset(vaddr, 0, pg_cnt + PG_SIZE);
  }
  return vaddr;
}

void mem_init(void)
{
  kputs("[INFO] Mem init start\n");
  mem_pool_init(get_ram());
  kputs("[INFO] Mem init done\n");
}
