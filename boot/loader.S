.code16
.text
.global loader
loader:
# Clear all the general registers
  xorw %ax, %ax
  xorw %bx, %bx
  xorw %cx, %cx
  xorw %dx, %dx

  movw $0x1300, %ax
  movw $0x0007, %bx
  movw $0xA, %cx
  movw $0, %dx
  movw $thanks, %bp
  int $0x10
  jmp .
  hlt
thanks:
  .ascii "I Love You"
