#include "vga.h"
#include "kio.h"
#include "cpuio.h"
#include <stdint.h>
static uint16_t put_backspace(uint16_t cursor)
{
  --cursor;
  *(uint16_t*)((uintptr_t)0xb8000+(cursor<<1)) = 0x0720;
  return cursor;
}
static uint16_t put_normal_char(uint16_t cursor, char const c)
{
  *(uint16_t*)((uintptr_t)0xb8000 + cursor*2) = 0x700 | c;
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
    *(uint16_t*)((uintptr_t)0xb8000+3840+i) = black_space;
  }
}
static uint16_t get_cursor(void)
{
  koutb(0x3d4, 0x0e);
  uint16_t cursor = (uint16_t)kinb(0x3d5) << 8;
  koutb(0x3d4, 0x0f);
  cursor |= kinb(0x3d5);
  return cursor;
}
// TODO CHANGE PAGE
void set_cursor(uint16_t cursor)
{
  koutb(0x3d4, 0x0e);
  koutb(0x3d5, cursor >> 8);
  koutb(0x3d4, 0x0f);
  koutb(0x3d5, cursor & 0xFF);
}
void vga_putc(char const c)
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
