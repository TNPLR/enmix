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
  push rsp
%endmacro

%macro popaq 0
  pop rsp
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
  mov rdi, %1
  extern general_interrupt
  call general_interrupt
  mov al, 0x20
  out 0xa0, al
  out 0x20, al

  popaq
  add rsp, 8
  iretq

section .data
  dq intr%1entry
%endmacro

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
