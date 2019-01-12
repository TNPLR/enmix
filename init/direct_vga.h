#ifndef DIRECT_VGA_H_
#define DIRECT_VGA_H_
#include <stdint.h>
#define VGA_RAM_START ((uint16_t *)0xB8000)
static uint16_t dvga_get_cursor(void) __attribute__((section(".text32")));
static void dvga_set_cursor(uint16_t) __attribute__((section(".text32")));
// void dvga_putchar(char);
#define dvga_putchar(x) dvga_putchar_color(x, 0x07)

static void dvga_putchar_color(char, uint8_t) __attribute__((section(".text32")));

static uint16_t dvga_get_cursor(void)
{
  uint16_t pos;
  /* Read high 8 bits of cursor position */
  outb(0x0E, 0x3D4);
  pos = inb(0x3D5) << 8;

  /* Read low 8 bits of cursor position */
  outb(0x0F, 0x3D4);
  pos |= inb(0x3D5);

  return pos;
}

static void dvga_set_cursor(uint16_t pos)
{
  outb(0x0E, 0x3D4);
  outb((uint8_t)(pos >> 8), 0x3D5);
  outb(0x0F, 0x3D4);
  outb((uint8_t)(pos & 0xFF), 0x3D5);
}

static void dvga_putchar_color(char c, uint8_t color)
{
  uint16_t cur = dvga_get_cursor();
  switch (c) {
    case '\n':
      cur = (cur + 80) % 80;
      break;
    default:
      *(VGA_RAM_START+cur) = (uint16_t)c | ((uint16_t)color << 8);
      ++cur;
      break;
  }

  if (cur >= 2000) {
    cur = 1920;
  }

  dvga_set_cursor(cur);
}
#endif /* DIRECT_VGA_H_ */
