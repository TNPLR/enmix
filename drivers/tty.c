#include "tty.h"
#include "vga.h"
#include "mutex_lock.h"
#include "thread.h"
#include <stdint.h>

static struct mutex_lock tty_lock;

void tty_init(void)
{
  mutex_lock_init(&tty_lock);
}

static void tty_request(void)
{
  mutex_lock_get(&tty_lock);
}

static void tty_release(void)
{
  mutex_lock_release(&tty_lock);
}

void tty_putc(const char c)
{
  tty_request();
  vga_putc(c);
  tty_release();
}

void tty_puts(const char * s)
{
  tty_request();
  while(*s) {
    vga_putc(*s);
    ++s;
  }
  tty_release();
}
