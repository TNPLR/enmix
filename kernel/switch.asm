[bits 64]
DEFAULT REL
section .text
extern kernel_thread
global switch_to
global entry_kernel_thread
switch_to:
  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx

  mov rax, rdi ; cur
  mov [rax], rsp ; save stack
  ; back up

  mov rax, rsi
  mov rsp, [rax]
  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15
  ret
entry_kernel_thread:
  push rbp
  mov rbp, rsp
  mov rdi, [rbp+80]
  mov rsi, [rbp+88]
  pop rbp
  jmp kernel_thread
