#ifndef INTERRUPT_H_
#define INTERRUPT_H_
#include <stdint.h>
void idt_init(void);
void setup_handler(uint8_t, void (*)(uint64_t, uint64_t));
void enable_interrupt(void);
void disable_interrupt(void);
int get_interrupt(void);
#endif
