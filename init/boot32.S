.code32
.section .text32, "ax"
.global _start
_start:
  cmpl $0x36d76289, %eax
  jne .no_multiboot
  movl $stack_top, %esp
  pushl %ebx
  .extern pmain
  call pmain
  jmp .
.no_multiboot:
  cli
  hlt
.skip 4096
stack_top:
