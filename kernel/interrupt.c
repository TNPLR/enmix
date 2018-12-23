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

static const char * interrupt_str[256];

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

static void interrupt_str_init(void)
{
  for (int i = 0; i < 256; ++i) {
    interrupt_str[i] = "Unknown";
  }
  interrupt_str[0] = "Divide Error";
  interrupt_str[1] = "Debug Exception";
  interrupt_str[2] = "NMI Interrupt";
  interrupt_str[3] = "Breakpoint";
  interrupt_str[4] = "Overflow";
  interrupt_str[5] = "BOUND Range Exceeded";
  interrupt_str[6] = "Invalid Opcode (Undefined Opcode)";
  interrupt_str[7] = "Device Not Available (No Math Coprocessor)";
  interrupt_str[8] = "Double Fault";
  interrupt_str[10] = "Invalid TSS";
  interrupt_str[11] = "Segment Not Present";
  interrupt_str[12] = "Stack-Segment Fault";
  interrupt_str[13] = "General Protection";
  interrupt_str[14] = "Page Fault";
  interrupt_str[16] = "x87 FPU Floating-Point Error (Math Fault)";
  interrupt_str[17] = "Alignment Check";
  interrupt_str[18] = "Machine Check";
  interrupt_str[19] = "SIMD Floating-Point Exception";
  interrupt_str[20] = "Virtualization Exception";
  kputs("[INFO] Interrupt strings set\n");
}

struct idt_ptr {
  uint16_t size;
  uint64_t offset;
} __attribute__((packed));

void idt_init()
{
  idt_desc_init();
  interrupt_str_init();
  struct idt_ptr tmp_idtptr;
  tmp_idtptr.size = sizeof(idt)-1;
  tmp_idtptr.offset = (uint64_t)idt;
  asm volatile("lidt %0"::"m"(tmp_idtptr));
  kputs("[INFO] idt_init done\n");
}

void general_interrupt(uint64_t s)
{
  kputs("Interrupt! No. ");
  kputuint(s, 10);
  kputs("--");
  kputs(interrupt_str[s%256]);
  kputs("\n");
}

static uint8_t timer_int_count = 0;
static uint64_t sec_count = 0;
void timer_interrupt(uint64_t s)
{
  if (++timer_int_count % 20 == 0) {
    kputuint(++sec_count,10);
    kputs(" sec.\r");
    timer_int_count = 0;
  }
}
void enable_interrupt(void)
{
  asm volatile("sti");
}

void disable_interrupt(void)
{
  asm volatile("cli");
}
