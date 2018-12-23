[bits 64]
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
  push rsp
%endmacro

%macro popaq 0
  pop rsp
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

%macro INTVECTOR 3
section .text
intr%1entry:
  %2
  pushaq
  mov rdi, %1
  extern %3
  call %3
  mov al, 0x20
  out 0xa0, al
  out 0x20, al

  popaq
  add rsp, 8
  iretq

section .data
  dq intr%1entry
%endmacro

INTVECTOR 0x00, ZERO, general_interrupt
INTVECTOR 0x01, ZERO, general_interrupt
INTVECTOR 0x02, ZERO, general_interrupt
INTVECTOR 0x03, ZERO, general_interrupt
INTVECTOR 0x04, ZERO, general_interrupt
INTVECTOR 0x05, ZERO, general_interrupt
INTVECTOR 0x06, ZERO, general_interrupt
INTVECTOR 0x07, ZERO, general_interrupt
INTVECTOR 0x08, ERROR_CODE, general_interrupt
INTVECTOR 0x09, ZERO, general_interrupt
INTVECTOR 0x0A, ERROR_CODE, general_interrupt
INTVECTOR 0x0B, ERROR_CODE, general_interrupt
INTVECTOR 0x0C, ERROR_CODE, general_interrupt
INTVECTOR 0x0D, ERROR_CODE, general_interrupt
INTVECTOR 0x0E, ERROR_CODE, general_interrupt
INTVECTOR 0x0F, ZERO, general_interrupt
INTVECTOR 0x10, ZERO, general_interrupt
INTVECTOR 0x11, ERROR_CODE, general_interrupt
INTVECTOR 0x12, ZERO, general_interrupt
INTVECTOR 0x13, ZERO, general_interrupt
INTVECTOR 0x14, ZERO, general_interrupt
INTVECTOR 0x15, ZERO, general_interrupt
INTVECTOR 0x16, ZERO, general_interrupt
INTVECTOR 0x17, ZERO, general_interrupt
INTVECTOR 0x18, ZERO, general_interrupt
INTVECTOR 0x19, ZERO, general_interrupt
INTVECTOR 0x1A, ZERO, general_interrupt
INTVECTOR 0x1B, ZERO, general_interrupt
INTVECTOR 0x1C, ZERO, general_interrupt
INTVECTOR 0x1D, ZERO, general_interrupt
INTVECTOR 0x1E, ZERO, general_interrupt
INTVECTOR 0x1F, ZERO, general_interrupt
INTVECTOR 0x20, ZERO, timer_interrupt
