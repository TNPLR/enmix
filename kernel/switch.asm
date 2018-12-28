[bits 64]
section .text
global switch_to
switch_to:
  push rsi
  push rdi
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
  pop rdi
  pop rsi
  ret
