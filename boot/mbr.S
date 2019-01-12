.code16
.section .loader
.global _start
_start:
# Initalize Segment registers as CS (CS should be 0x0)
  movw %cs, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %gs
  movw %ax, %ss

# Use 0x10 interrupt to write a character
  movb $0x09, %ah   # Write character
  movb $'M', %al    # Print 'R'
  movw $0x0007, %bx # Background: Black; Text: Light gray
  movw $1, %cx      # Print one character
  int $0x10

# Use 0x13 interrupt to read 3 sectors (second stage loader)
# Clear registers
  xorw %ax, %ax
  xorw %dx, %dx
  xorw %si, %si

  movb $0x42, %ah   # Extended Read Sectors From Drive
  movb $0x80, %dl   # First Hard disk
  movw $DAC, %si
  int $0x13

  movw $0x900, %ax
  jmp *%ax
ERROR:
  cli # Clear interrupt flag
  hlt # Halt CPU
DAC:
  .byte 0x10 # Sizeof DAC
  .byte 0 # unused
  .word 1 # Read 1 sectors (FAT Boot record)
  .long 0x900
  .quad 0x2 # from sector 1 (the third sector)
.org 446, 0x0
# MBR
# Active
.byte 0x80
# CHS Start Pos (NULL in miros)
.byte 0
.byte 0
.byte 0
# Partition Type
.byte 0x83 # Any Linux Partition
# CHS End Pos (NULL in miros)
.byte 0
.byte 0
.byte 0
# LBA Start Pos
.long 1  # MBR needs 1 sectors
# Sectors count
.long 98303  # 98304 - 1
.org 510, 0x0
.byte 0x55 # Boot Flags
.byte 0xaa
