global put_test
put_test:
  mov ax, 0xb800
  mov gs, ax
  push bx
  mov bx, 0x0
  mov word [gs:bx], 0x0741
  pop bx
  ret
