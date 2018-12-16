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
    pputs("[ERROR] CPU DOES NOT support CPUID\n");
    goto long_mode_error;
  }
  if (!cpuid_extended_check_long()) {
    pputs("[ERROR] CPU DOES NOT support long-mode-test CPUID\n");
    goto long_mode_error;
  }
  if (!cpuid_detect_long_mode()) {
    pputs("[ERROR] CPU DOES NOT support long-mode\n");
    goto long_mode_error;
  }
  pputs("[INFO] Long-mode-checking is done\n");
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
const uint32_t PML4E = 0x1000;
#define PAGE_SIZE 0x1000
static void setup_paging(void)
{
  clear_mem((void *)PML4E, PAGE_SIZE * 4);
  set_cr3(PML4E);
  *(uint32_t *)PML4E = PML4E + PAGE_SIZE + 0x7;
  *(uint32_t *)(PML4E + PAGE_SIZE) = PML4E + 2 * PAGE_SIZE + 0x7;
  *(uint32_t *)(PML4E + 2 * PAGE_SIZE) = PML4E + 3 * PAGE_SIZE + 0x7;
  uint32_t *page_table_ptr = (uint32_t *)(PML4E + 3 * PAGE_SIZE);
  uint32_t pt_index = 0x3;
  for (uint32_t i = 0; i < 256; ++i) {
    *page_table_ptr = pt_index;
    pt_index += PAGE_SIZE;
    page_table_ptr += 2; // Because PT is 64 bit
  }
  pputs("[INFO] Paging done\n");
}
int pmain()
{
  set_cursor(400);
  pputs("[KERNEL]\n[INFO] Kernel initalizing...\n");
  
  check_long_mode();
  
  setup_paging();
  while (1);
  return 0;
}
