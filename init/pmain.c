#include <stdint.h>
#include "mix.h"
#define KERNEL_READ_ADDR 0x800000
#define KERNEL_BIN_ADDR 0xc0000000U
#define PAGE_SIZE 0x1000
#define KERNEL_LOAD_ADDR 0x100000
#define KERNEL_SIZE 4
#define KERNEL_STACK 0xc0204000U
const uint32_t PML4T = 0x1000;
const uint32_t KERNEL_PDE = 0x6000;

static inline uint32_t get_flags(void)
{
  uint32_t ret_val;
  asm volatile("pushfl;popl %%eax":"=a"(ret_val)::"memory");
  return ret_val;
}
static inline void set_flags(uint32_t flag)
{
  asm volatile("pushl %%eax; popfl"::"a"(flag):"memory");
}
struct cpuid_registers {
  uint32_t eax, ebx, ecx, edx;
};
static inline struct cpuid_registers cpuid(uint32_t num)
{
  struct cpuid_registers ret_val;
  asm volatile("cpuid":"=a"(ret_val.eax),"=b"(ret_val.ebx),
      "=c"(ret_val.ecx),"=d"(ret_val.edx):"a"(num));
  return ret_val;
}
static int test_cpuid_long(void)
{
  uint32_t flags = get_flags();
  uint32_t tmp_flags = flags;
  tmp_flags ^= 1 << 21;
  set_flags(tmp_flags);
  tmp_flags = get_flags();
  set_flags(flags);
  return flags != tmp_flags;
}
static int cpuid_extended_check_long(void)
{
  struct cpuid_registers tmp = cpuid(0x80000000);
  return tmp.eax >= 0x80000001;
}
static int cpuid_detect_long_mode(void)
{
  struct cpuid_registers tmp = cpuid(0x80000001);
  return tmp.edx & (1 << 29);
}
static void check_long_mode(void)
{
  if (!test_cpuid_long()) {
    pputs("[ERR] CPU DOES NOT support CPUID\n");
    goto long_mode_error;
  }
  if (!cpuid_extended_check_long()) {
    pputs("[ERR] CPU DOES NOT support long-mode-test CPUID\n");
    goto long_mode_error;
  }
  if (!cpuid_detect_long_mode()) {
    pputs("[ERR] CPU DOES NOT support long-mode\n");
    goto long_mode_error;
  }
  pputs("[INFO] Long-mode-checking is done\n");
  return;
long_mode_error:
  pputs("[EMERG] MIROS CANNOT BE BOOTED\n");
  while(1);
}
static void clear_mem(void *position, uint32_t byte_count)
{
  while (byte_count--) {
    *((uint8_t *)position+byte_count) = 0;
  }
}
static void set_cr3(uint32_t val)
{
  asm volatile("movl %0, %%cr3"::"a"(val));
}
static void setup_paging(void)
{
  clear_mem((void *)PML4T, PAGE_SIZE * 5);
  set_cr3(PML4T);
  pputs("[INFO] CR3 Register set\n");
  *(uint32_t *)PML4T = PML4T + PAGE_SIZE + 0x7;
  // Loop last PML4E to PML4T itself
  *(uint32_t *)(PML4T + 8 * 511) = PML4T + 0x7;
  *(uint32_t *)(PML4T + PAGE_SIZE) = PML4T + 2 * PAGE_SIZE + 0x7;
  // first mib
  *(uint32_t *)(PML4T + 2 * PAGE_SIZE) = PML4T + 3 * PAGE_SIZE + 0x7;
  uint32_t *page_table_ptr = (uint32_t *)(PML4T + 3 * PAGE_SIZE);
  uint32_t pt_index = 0x3;
  for (uint32_t i = 0; i < 256; ++i) {
    *page_table_ptr = pt_index;
    pt_index += PAGE_SIZE;
    page_table_ptr += 2; // Because PT is 64 bit
  }
  // 8th and 9th mib
  *(uint32_t *)(PML4T + 2 * PAGE_SIZE + 4*8) = PML4T + 4 * PAGE_SIZE + 0x7;
  page_table_ptr = (uint32_t *)(PML4T + 4 * PAGE_SIZE);
  pt_index = 0x800003;
  for (uint32_t i = 0; i < 512; ++i) {
    *page_table_ptr = pt_index;
    pt_index += PAGE_SIZE;
    page_table_ptr += 2; // Because PT is 64 bit
  }
  pputs("[INFO] Paging done\n");
}

static void kernel_paging(uint32_t kernel_mb_count)
{
  clear_mem((void *)KERNEL_PDE, PAGE_SIZE * 3);
  uint32_t kernel_page_count = kernel_mb_count * 256;
  // 3rd entry of PML4
  *(uint32_t *)(PML4T+PAGE_SIZE+8*3) = KERNEL_PDE + 0x7;
  *(uint32_t *)(KERNEL_PDE) = KERNEL_PDE + PAGE_SIZE + 0x7;
  uint32_t now_ptable = 0;
  uint32_t *ptable_ptr = (uint32_t *)(KERNEL_PDE + PAGE_SIZE);
  uint32_t pt_index = 0x100003;
  for (uint32_t i = 0; i < kernel_page_count; ++i) {
    *ptable_ptr = pt_index;
    pt_index += PAGE_SIZE;
    ptable_ptr += 2;
    if (i % 512 == 0) {
      ++now_ptable;
      *(uint32_t *)(KERNEL_PDE + now_ptable*8) = 
        KERNEL_PDE + (1 + now_ptable) * PAGE_SIZE + 0x7;
    }
  }
  pputs("[INFO] Kernel paging configured\n");
}
static void pmem_cpy(char *dst, char const *src, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i) {
    *dst++ = *src++;
  }
}
static uint32_t copy_kernel(void)
{
  uint32_t program_header = *(uint32_t *)(KERNEL_READ_ADDR+32);
  program_header += KERNEL_READ_ADDR;
  uint8_t program_header_count = *(uint8_t *)(KERNEL_READ_ADDR+56);
  uint8_t program_header_size = *(uint8_t *)(KERNEL_READ_ADDR+54);
  for (int i = 0; i < program_header_count; ++i) {
    if (*(uint32_t *)program_header == 0) {
      program_header += program_header_size;
      continue;
    }
    pmem_cpy((char *)(*(uint32_t *)(program_header+0x10)),
        (char *)(*(uint32_t *)(program_header+0x08)+KERNEL_READ_ADDR),
        *(uint32_t *)(program_header+0x20));
    program_header += program_header_size;
  }
  pputs("[INFO] Kernel loadin\n");
  return *(uint32_t *)(KERNEL_READ_ADDR + 0x18);
}
static void start_pae(void)
{
  asm volatile("movl %%cr4, %%eax; orl %0, %%eax;"
      "movl %%eax, %%cr4"::"i"(1<<5):"eax");
  pputs("[INFO] PAE enabled\n");
}
static void enter_longmode(void)
{
  asm volatile("rdmsr; orl %0, %%eax; wrmsr"::"i"(1<<8),"c"(0xC0000080):"ax");
  pputs("[INFO] Longmode enabled\n");
}
static void enable_paging(void)
{
  asm volatile("movl %%cr0, %%eax; orl %0, %%eax;"
      "movl %%eax, %%cr0"::"i"(1<<31):"ax");
  pputs("[INFO] Paging enabled\n");
}
static _Noreturn void jump_long(uint32_t entry)
{
  pputs("[INFO] Jump to longmode\n");
  asm volatile("ljmp $0x20, $USELESS_LABEL;USELESS_LABEL:");
  asm volatile("movl %%edx, %%esp; jmpl *%%eax"::"d"(0xc0100000U),"a"(entry));
  __builtin_unreachable();
}
int pmain()
{
  set_cursor(400);
  pputs("[KERNEL]\n[INFO] Kernel initalizing...\n");
  
  check_long_mode();
  
  setup_paging();

  kernel_paging(KERNEL_SIZE); // 2mb kernel

  start_pae();

  enter_longmode();

  enable_paging();

  uint32_t kernel_entry = copy_kernel();
  jump_long(kernel_entry);
  while (1);
  return 0;
}
