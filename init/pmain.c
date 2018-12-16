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
static uint16_t put_backspace(uint16_t cursor)
{
  --cursor;
  *(uint16_t*)((uint32_t)0xb8000+(cursor<<1)) = 0x0720;
  return cursor;
}
static uint16_t put_normal_char(uint16_t cursor, char const c)
{
  *(uint16_t*)((uint32_t)0xb8000 + cursor*2) = 0x700 | c;
  return cursor+1;
}
static uint16_t put_carriage_return(uint16_t cursor)
{
  cursor -= cursor % 80;
  return cursor;
}
static uint16_t put_new_line(uint16_t cursor)
{
  cursor += 80 - cursor % 80;
  return cursor;
}
static void roll_screen(void)
{
  uint32_t *dest = (uint32_t *)0xb8000;
  uint32_t *src = (uint32_t *)0xb80a0;
  for (int i = 0; i < 960; ++i) {
    *dest++ = *src++;
  }
}
static void clear_last_line(void)
{
  uint16_t black_space = 0x0720;
  for (int i = 0; i < 160; i+=2) {
    *(uint16_t*)((uint32_t)3840+i) = black_space;
  }
}
static uint16_t get_cursor(void)
{
  poutb(0x3d4, 0x0e);
  uint16_t cursor = (uint16_t)pinb(0x3d5) << 8;
  poutb(0x3d4, 0x0f);
  cursor |= pinb(0x3d5);
  return cursor;
}
static void set_cursor(uint16_t cursor)
{
  poutb(0x3d4, 0x0e);
  poutb(0x3d5, cursor >> 8);
  poutb(0x3d4, 0x0f);
  poutb(0x3d5, cursor & 0xFF);
}
static void pputc(char const c)
{
  uint16_t cursor = get_cursor();
  switch (c) {
    case '\r':
      cursor = put_carriage_return(cursor);
      break;
    case '\n':
      cursor = put_new_line(cursor);
      break;
    case '\b':
      cursor = put_backspace(cursor);
      break;
    default:
      cursor = put_normal_char(cursor, c);
      break;
  }
  if (cursor >= 2000) {
    roll_screen();
    clear_last_line();
    cursor = 1920;
  }
  set_cursor(cursor);
}
static void pputs(char const *s)
{
  while(*s) {
    pputc(*s);
    ++s;
  }
}
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
  pputs("[EMERG] Long-mode-checking is done\n");
  return;
long_mode_error:
  pputs("[ERROR] MIROS CANNOT BE BOOTED\n");
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
const uint32_t PML4T = 0x1000;
const uint32_t KERNEL_PDPTE = 0x5000;
#define PAGE_SIZE 0x1000
static void setup_paging(void)
{
  clear_mem((void *)PML4T, PAGE_SIZE * 4);
  set_cr3(PML4T);
  pputs("[INFO] CR3 Register set\n");
  *(uint32_t *)PML4T = PML4T + PAGE_SIZE + 0x7;
  *(uint32_t *)(PML4T + PAGE_SIZE) = PML4T + 2 * PAGE_SIZE + 0x7;
  *(uint32_t *)(PML4T + 2 * PAGE_SIZE) = PML4T + 3 * PAGE_SIZE + 0x7;
  uint32_t *page_table_ptr = (uint32_t *)(PML4T + 3 * PAGE_SIZE);
  uint32_t pt_index = 0x3;
  for (uint32_t i = 0; i < 256; ++i) {
    *page_table_ptr = pt_index;
    pt_index += PAGE_SIZE;
    page_table_ptr += 2; // Because PT is 64 bit
  }
  pputs("[INFO] Paging done\n");
}

#define KERNEL_LOAD_ADDR 0x100000
static void kernel_paging(uint32_t kernel_mb_count)
{
  uint32_t kernel_page_count = kernel_mb_count * 256;
  // 7th entry of PML4
  *(uint32_t *)(PML4T+8*7) = KERNEL_PDPTE;
  *(uint32_t *)(KERNEL_PDPTE) = KERNEL_PDPTE + PAGE_SIZE + 0x7;
  *(uint32_t *)(KERNEL_PDPTE + PAGE_SIZE) = KERNEL_PDPTE + 2 * PAGE_SIZE + 0x7;
  uint32_t now_ptable = 0;
  uint32_t *ptable_ptr = (uint32_t *)(KERNEL_PDPTE + 2 * PAGE_SIZE);
  uint32_t pt_index = 100003;
  for (uint32_t i = 1; i <= kernel_page_count; ++i) {
    *ptable_ptr = pt_index;
    pt_index += PAGE_SIZE;
    ptable_ptr += 2;
    if (i % 512 == 0) {
      ++now_ptable;
      *(uint32_t *)(KERNEL_PDPTE + PAGE_SIZE + now_ptable*8) = 
        KERNEL_PDPTE + (2 + now_ptable) * PAGE_SIZE + 0x7;
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
#define KERNEL_READ_ADDR 0x800000
#define KERNEL_BIN_ADDR 0xc0000000
static void copy_kernel(void)
{
  uint32_t program_header = *(uint32_t *)(KERNEL_READ_ADDR+32);
  program_header += KERNEL_BIN_ADDR;
  uint32_t program_header_count = *(uint32_t *)(KERNEL_READ_ADDR+56);
  uint32_t program_header_size = *(uint32_t *)(KERNEL_READ_ADDR+54);
  for (int i = 0; i < program_header_count; ++i) {
    if (*(uint8_t *)program_header == 0) {
      program_header += program_header_size;
      continue;
    }
    pmem_cpy((char *)*(uint32_t *)(program_header+0x10),
        (char *)*(uint32_t *)(program_header+0x08) + KERNEL_READ_ADDR,
        *(uint32_t *)(program_header+0x20));
    program_header += program_header_size;
  }
  pputs("[INFO] Kernel loadin\n");
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
      "mov %%eax, %%cr0"::"i"(1<<31):"ax");
  pputs("[INFO] Paging enabled\n");
}
#define KERNEL_SIZE 2
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

  copy_kernel();
  while (1);
  return 0;
}
