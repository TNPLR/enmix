.code16
.section .text
# Use 0x10 interrupt to write a character
movb $0x09, %ah # Write character
movb $'R', %al # Print 'R'
movw $0x0007, %bx # Background: Black; Text: Light gray
movw $1, %cx # Print one character
int $0x10
cli # Clear interrupt flag
hlt # Halt CPU

.org 510, 0x0
.byte 0x55 # Boot Flags
.byte 0xaa
