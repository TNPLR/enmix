#ifndef VGA_H_
#define VGA_H_
#include <stdint.h>
void vga_putc(char const c);
void set_cursor(uint16_t cursor);
#endif
