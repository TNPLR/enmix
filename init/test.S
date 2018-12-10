SECTION .text vstart=0x900
HEHE:
  mov ax, 0xb800
  mov gs, ax
  mov bx, 0x0
  mov word [gs:bx], 0x074C
  add bx, 2
  mov word [gs:bx], 0x074F
  add bx, 2
  mov word [gs:bx], 0x0741
  add bx, 2
  mov word [gs:bx], 0x0744
  add bx, 2
  mov word [gs:bx], 0x0745
  add bx, 2
  mov word [gs:bx], 0x0752
  jmp $
