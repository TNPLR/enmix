#include "keyboard.h"
#include "kio.h"
#include "interrupt.h"
#include "cpuio.h"
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
  kputc('K');
  kinb(KEYBOARD_BUF_PORT);
}

void keyboard_init(void)
{
  setup_handler(0x21, (void (*)(uint64_t,uint64_t))intr_keyboard_handler);
  kputs("[INFO] Keyboard init done\n");
}
