[bits 64]
DEFAULT REL
%define ERROR_CODE nop
%define ZERO push 0

section .data
global intr_entry_table
intr_entry_table:

%macro pushaq 0
  push rax
  push rbx
  push rcx
  push rdx
  push rbp
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
%endmacro

%macro popaq 0
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax
%endmacro

%macro INTVECTOR 2
section .text
intr%1entry:
  %2
  pushaq
  mov al, 0x20
  out 0xa0, al
  out 0x20, al
  mov rdi, %1
  extern idt_func_table
  call [idt_func_table + %1 * 8]
  jmp intr_ret

section .data
  dq intr%1entry
%endmacro

section .text
global intr_ret
intr_ret:
  popaq
  add rsp, 8
  iret

INTVECTOR 0x00, ZERO
INTVECTOR 0x01, ZERO
INTVECTOR 0x02, ZERO
INTVECTOR 0x03, ZERO
INTVECTOR 0x04, ZERO
INTVECTOR 0x05, ZERO
INTVECTOR 0x06, ZERO
INTVECTOR 0x07, ZERO
INTVECTOR 0x08, ERROR_CODE
INTVECTOR 0x09, ZERO
INTVECTOR 0x0A, ERROR_CODE
INTVECTOR 0x0B, ERROR_CODE
INTVECTOR 0x0C, ERROR_CODE
INTVECTOR 0x0D, ERROR_CODE
INTVECTOR 0x0E, ERROR_CODE
INTVECTOR 0x0F, ZERO
INTVECTOR 0x10, ZERO
INTVECTOR 0x11, ERROR_CODE
INTVECTOR 0x12, ZERO
INTVECTOR 0x13, ZERO
INTVECTOR 0x14, ZERO
INTVECTOR 0x15, ZERO
INTVECTOR 0x16, ZERO
INTVECTOR 0x17, ZERO
INTVECTOR 0x18, ZERO
INTVECTOR 0x19, ZERO
INTVECTOR 0x1A, ZERO
INTVECTOR 0x1B, ZERO
INTVECTOR 0x1C, ZERO
INTVECTOR 0x1D, ZERO
INTVECTOR 0x1E, ZERO
INTVECTOR 0x1F, ZERO
INTVECTOR 0x20, ZERO
INTVECTOR 0x21, ZERO
INTVECTOR 0x22, ZERO
INTVECTOR 0x23, ZERO
INTVECTOR 0x24, ZERO
INTVECTOR 0x25, ZERO
INTVECTOR 0x26, ZERO
INTVECTOR 0x27, ZERO
INTVECTOR 0x28, ZERO
INTVECTOR 0x29, ZERO
INTVECTOR 0x2A, ZERO
INTVECTOR 0x2B, ZERO
INTVECTOR 0x2C, ZERO
INTVECTOR 0x2D, ZERO
INTVECTOR 0x2E, ZERO
