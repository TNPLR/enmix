.code16
.text
.global loader
loader:
# Clear all the general registers
  xorw %ax, %ax
  xorw %bx, %bx
  xorw %cx, %cx
  xorw %dx, %dx

# Move stack to 0x900
  movw $0x900, %ax
  movw %ax, %ss

  movw $0xA, %cx
  movw $thanks, %bp
  call println
  jmp .
  hlt

thanks:
  .ascii "I Love You"

# Print string
# Registers:
#   cx: Characters count
#   bp: Address of the string
println:
# Get cursor
  pushw %cx
  movw $0, %bx
  movb $0x03, %ah
  int $0x10

# Print string
  popw %cx
  movw $0x1300, %ax
  movw $0x0007, %bx
  int $0x10

# Move to next line
  movb $0x02, %ah
  movw $0, %bx
  addb $1, %dh
  movb $0, %dl
  int $0x10
  ret
