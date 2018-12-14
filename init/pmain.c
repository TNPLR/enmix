#include <stdint.h>
static void poutb(uint16_t port, uint8_t value)
{
  asm volatile("outb %%al, %%dx"::"a"(value), "d"(port));
}
static uint8_t pinb(uint16_t port)
{
  uint8_t ret_val;
  asm volatile("inb %%dx, %%al":"=a"(ret_val):"d"(port));
  return ret_val;
}
static uint16_t pinw(uint16_t port)
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
  *(uint16_t*)((uint32_t)0xb8000+(cursor<<1)) = 0x700 | c;
  return cursor+1;
}
static uint16_t put_carriage_return(uint16_t cursor)
{
  cursor -= cursor % 80;
  return cursor;
}
static uint16_t put_new_line(uint16_t cursor)
{
  cursor -= cursor % 80 + 80;
  return cursor;
}
static void roll_screen(void)
{
  uint32_t *dest = 0xb8000;
  uint32_t *src = 0xb80a0;
  for (int i = 0; i < 960; ++i) {
    *dest++ = *src++;
  }
}
static void clear_last_line(void)
{
  uint16_t black_space = 0x0720;
  for (int i = 0; i < 160; i+=2) {
    *(uint16_t*)(3840+i) = black_space;
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
int pmain()
{
  pputc('D');
  pputc('A');
  pputc('C');
  while (1);
  return 0;
}
