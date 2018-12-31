#include "keyboard.h"
#include "kio.h"
#include "interrupt.h"
#include "cpuio.h"
#include "iobuffer.h"
#include <stdint.h>

#define KEYBOARD_BUF_PORT 0x60

#define ESC '\x1B'
#define BACKSPACE '\b'
#define TAB '\t'
#define ENTER '\n' // New line, not return
#define DELETE '\x7F'

// Control Keys
#define INVISIBLE_CHAR 0

#define LCTRL_CHAR INVISIBLE_CHAR
#define RCTRL_CHAR INVISIBLE_CHAR
#define LSHIFT_CHAR INVISIBLE_CHAR
#define RSHIFT_CHAR INVISIBLE_CHAR
#define LALT_CHAR INVISIBLE_CHAR
#define RALT_CHAR INVISIBLE_CHAR
#define CAPSLOCK_CHAR INVISIBLE_CHAR

#define LSHIFT_MAKE 0x2A
#define LSHIFT_BREAK 0xAA
#define RSHIFT_MAKE 0x36
#define RSHIFT_BREAK 0xB6
#define LALT_MAKE 0x38
#define LALT_BREAK 0xB8
#define RALT_MAKE 0xE038
#define RALT_BREAK 0xE0B8
#define LCTRL_MAKE 0x1D
#define LCTRL_BREAK 0x9D
#define RCTRL_MAKE 0xE01D
#define RCTRL_BREAK 0xE09D
#define CAPSLOCK_MAKE 0x3A

static int ctrl_status;
static int shift_status;
static int alt_status;
static int capslock_status;
static int extend_scancode;
struct iobuffer keyboard_buf;

/* Shift or not */
static char keymap[][2] = {
  {0, 0},
  {ESC, ESC},
  {'1', '!'},
  {'2', '@'},
  {'3', '#'},
  {'4', '$'},
  {'5', '%'},
  {'6', '^'},
  {'7', '&'},
  {'8', '*'},
  {'9', '('},
  {'0', ')'},
  {'-', '_'},
  {'=', '+'},
  {BACKSPACE, BACKSPACE},
  {TAB, TAB},
  {'q', 'Q'},
  {'w', 'W'},
  {'e', 'E'},
  {'r', 'R'},
  {'t', 'T'},
  {'y', 'Y'},
  {'u', 'U'},
  {'i', 'I'},
  {'o', 'O'},
  {'p', 'P'},
  {'[', '{'},
  {']', '}'},
  {ENTER, ENTER},
  {LCTRL_CHAR, LCTRL_CHAR},
  {'a', 'A'},
  {'s', 'S'},
  {'d', 'D'},
  {'f', 'F'},
  {'g', 'G'},
  {'h', 'H'},
  {'j', 'J'},
  {'k', 'K'},
  {'l', 'L'},
  {';', ':'},
  {'\'', '"'},
  {'`', '~'},
  {LSHIFT_CHAR, LSHIFT_CHAR},
  {'\\', '|'},
  {'z', 'Z'},
  {'x', 'X'},
  {'c', 'C'},
  {'v', 'V'},
  {'b', 'B'},
  {'n', 'N'},
  {'m', 'M'},
  {',', '<'},
  {'.', '>'},
  {'/', '?'},
  {RSHIFT_CHAR},
  {'*', '*'},
  {LALT_CHAR, LALT_CHAR},
  {' ', ' '},
  {CAPSLOCK_CHAR, CAPSLOCK_CHAR}
};

static void intr_keyboard_handler(void)
{
  int ctrl_down_last = ctrl_status;
  int shift_down_last = shift_status;
  int capslock_last = capslock_status;
  int break_code;
  uint16_t scancode = kinb(KEYBOARD_BUF_PORT);
  if (scancode == 0xe0) {
    extend_scancode = 1;
    return;
  }

  if (extend_scancode) {
    scancode |= 0xe000;
    extend_scancode = 0;
  }

  break_code = ((scancode & 0x0080) != 0); //detect break code
  if (break_code) {
    uint16_t make_code = (scancode &= 0xff7f);
    if (make_code == LCTRL_MAKE || make_code == RCTRL_MAKE) {
      ctrl_status = 0;
    } else if (make_code == LSHIFT_MAKE || make_code == RSHIFT_MAKE) {
      shift_status = 0;
    } else if (make_code == LALT_MAKE || make_code == RALT_MAKE) {
      alt_status = 0;
    }
    return;
  } else if ((scancode > 0x00 && scancode < 0x3B) ||
      (scancode == RALT_MAKE) || (scancode == RCTRL_MAKE)) {
    int shift = 0;
    if ((scancode < 0x0E) || (scancode == 0x29) || (scancode == 0x1A) ||
        (scancode == 0x1B) || (scancode == 0x2B) || (scancode == 0x27) ||
        (scancode == 0x28) || (scancode == 0x33) || (scancode == 0x34) ||
        (scancode == 0x35)) {
      if (shift_down_last) {
        shift = 1;
      }
    } else {
      if (shift_down_last && capslock_last) {
        shift = 0;
      } else if (shift_down_last || capslock_last) {
        shift = 1;
      } else {
        shift = 0;
      }
    }
    uint8_t index = (scancode &= 0xFF);

    char cur_char = keymap[index][shift];
    if (cur_char) {
      if (!iobuffer_full(&keyboard_buf)) {
        iobuffer_putchar(&keyboard_buf, cur_char);
      }
      return;
    }

    if (scancode == LCTRL_MAKE || scancode == RCTRL_MAKE) {
      ctrl_status = 1;
    } else if (scancode == LSHIFT_MAKE || scancode == RSHIFT_MAKE) {
      shift_status = 1;
    } else if (scancode == LALT_MAKE || scancode == RALT_MAKE) {
      alt_status = 1;
    } else if (scancode == CAPSLOCK_MAKE) {
      capslock_status = !capslock_status;
    }
  } else {
    kputs("[ERR] Unknown Key\n");
  }
}

void keyboard_init(void)
{
  iobuffer_init(&keyboard_buf);
  setup_handler(0x21, (void (*)(uint64_t,uint64_t))intr_keyboard_handler);
  kputs("[INFO] Keyboard init done\n");
}
