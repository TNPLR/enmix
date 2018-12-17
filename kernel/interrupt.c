#include "interrupt.h"
#include "kio.h"
#include <stdint.h>
#define INTERRUPT_DESC_SETTING 0x8E
#define IDT_DESC_CNT 0x21
#define CODE_64_SELECTOR 0x20
struct gate_desc {
  uint16_t offset_low16;
  uint16_t segment_selector;
  uint8_t ist:3;
  uint8_t zero:5;
  uint8_t setting;
  uint16_t offset_midlow16;
  uint32_t offset_high32;
  uint32_t reserved;
};

static struct gate_desc idt[IDT_DESC_CNT];

extern void *intr_entry_table[IDT_DESC_CNT];

static void make_idt_desc(struct gate_desc *idt_gate,
    uint8_t setting, void *function)
{
  idt_gate->offset_low16 = (uint64_t)function & 0xFFFF;
  idt_gate->segment_selector = CODE_64_SELECTOR;
  idt_gate->ist = 0;
  idt_gate->zero = 0;
  idt_gate->setting = setting;
  idt_gate->offset_midlow16 = ((uint64_t)function >> 16) & 0xFFFF;
  idt_gate->offset_high32 = (uint64_t)function >> 32;
  idt_gate->reserved = 0;
}

static void idt_desc_init(void)
{
  for (int i = 0; i < IDT_DESC_CNT; ++i) {
    make_idt_desc(&idt[i],INTERRUPT_DESC_SETTING,intr_entry_table[i]);
  }
  kputs("[INFO] Interrupt set\n");
}

struct idt_ptr {
  uint16_t size;
  uint64_t offset;
} __attribute__((packed));

void idt_init()
{
  idt_desc_init();
  struct idt_ptr tmp_idtptr;
  tmp_idtptr.size = sizeof(idt)-1;
  tmp_idtptr.offset = (uint64_t)idt;
  asm volatile("lidt %0"::"m"(tmp_idtptr));
  kputs("[INFO] idt_init done\n");
}

void general_interrupt(uint64_t s)
{
  kputs("Interrupt !! No");
  kputc((s+0x41));
  kputs(".....\n");
}

void enable_interrupt(void)
{
  asm volatile("sti");
}
