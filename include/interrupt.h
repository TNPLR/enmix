#ifndef INTERRUPT_H_
#define INTERRUPT_H_
#include <stdint.h>
void idt_init(void);
void enable_interrupt(void);
void disable_interrupt(void);
void setup_handler(uint8_t, void (*)(uint64_t));
#endif
