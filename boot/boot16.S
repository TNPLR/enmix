.code16
.section .text
boot16:
  inb $0x92, %al
  orb $2, %al
  outb %al, $0x92

  cli
  lgdt gdt_ptr

  movl %cr0, %eax
  orl $1, %eax
  movl %eax, %cr0

  ljmp $0x08, $pmode

gdt_base:
  .long 0x0
  .long 0x0
  .long 0x0000FFFF
  .long 0x00CF9A00
  .long 0x0000FFFF
  .long 0x00CF9200
gdt_end:
gdt_ptr:
  .word gdt_end - gdt_base - 1
  .long gdt_base
.code32
pmode:
  movw $0x10, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss
  movb $'P',0xB80A0
  movb $0x07, 0xB80A1
  jmp .
