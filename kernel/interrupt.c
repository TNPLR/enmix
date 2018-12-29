#include "interrupt.h"
#include "thread.h"
#include "kio.h"
#include "vga.h"
#include <stdint.h>
#include <stddef.h>
#include "assert.h"
#define INTERRUPT_DESC_SETTING 0x8E
#define INTER
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
void general_interrupt(uint64_t s, uint64_t err)
{
  if (s == 0x27 || s == 0x2f) {
    return;
  } // fake interrupt
  set_cursor(0);
  for (int i = 0; i < 20; ++i) {
    kputs("                                                  \n");
  }
  set_cursor(0);
  kputs("**************************************************\n");
  kputs("[EMERG] EXCEPTION MASSAGE\n");
  kputs("**************************************************\n");
  kputs("Interrupt! No. ");
  kputuint(s, 10);
  kputs("--");
  kputs(interrupt_str[s%256]);
  if (s == 14) {// page fault
    uint64_t page_fault_vaddr;
    asm("movq %%cr2, %0":"=r"(page_fault_vaddr));
    kputs("\nPAGE_FAULT_ADDR = 0x");
    kputuint(page_fault_vaddr, 16);
  }
  kputs("\nError Number: 0x");
  kputuint(err, 16);
  kputs("\n**************************************************\n");
  kputs("[EMERG] EXCEPTION MASSAGE END\n");
  kputs("**************************************************\n");
  while (1);
}

//void (*idt_func_table[256])(uint64_t) = {general_interrupt};
void (*idt_func_table[256])(uint64_t,uint64_t) = {general_interrupt};
static struct gate_desc idt[IDT_DESC_CNT];

extern void *intr_entry_table[IDT_DESC_CNT];

static void make_idt_desc(struct gate_desc *idt_gate,
    uint8_t setting, void *function, uint8_t ist)
{
  idt_gate->offset_low16 = (uint64_t)function & 0xFFFF;
  idt_gate->segment_selector = CODE_64_SELECTOR;
  idt_gate->ist = ist & 0x7;
  idt_gate->zero = 0;
  idt_gate->setting = setting;
  idt_gate->offset_midlow16 = ((uint64_t)function >> 16) & 0xFFFF;
  idt_gate->offset_high32 = (uint64_t)function >> 32;
  idt_gate->reserved = 0;
}

static void idt_desc_init(void)
{
  for (int i = 0; i < IDT_DESC_CNT; ++i) {
    make_idt_desc(&idt[i],INTERRUPT_DESC_SETTING,intr_entry_table[i],0);
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

void setup_handler(uint8_t int_no, void (*func)(uint64_t,uint64_t))
{
  idt_func_table[int_no] = func;
}

struct idt_ptr {
  uint16_t size;
  uint64_t offset;
} __attribute__((packed));

static uint64_t tick;
void timer_interrupt(void)
{
  struct task_struct * cur_thread = running_thread();
  ASSERT(cur_thread->stack_magic == 0x52735273);

  ++cur_thread->elapsed_ticks;
  ++tick;
  if (cur_thread->ticks == 0) {
    schedule();
  } else {
    --cur_thread->ticks;
  }
}

static void init_handler_function(void)
{
  setup_handler(0x20, (void (*)(uint64_t, uint64_t))timer_interrupt);
  kputs("[INFO] Init all the interrupt handler functions\n");
}

void idt_init()
{
  idt_desc_init();
  interrupt_str_init();
  init_handler_function();
  struct idt_ptr tmp_idtptr;
  tmp_idtptr.size = sizeof(idt)-1;
  tmp_idtptr.offset = (uint64_t)idt;
  asm volatile("lidt %0"::"m"(tmp_idtptr));
  kputs("[INFO] idt_init done\n");
}

void enable_interrupt(void)
{
  asm volatile("sti");
}

void disable_interrupt(void)
{
  asm volatile("cli");
}

int get_interrupt(void)
{
  uint64_t ret;
  asm volatile("pushfq; popq %%rax":"=a"(ret));
  return (int)ret & 0x200;
}
