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
  movb $'R', %al    # Print 'R'
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
  .word 3 # Read 3 sectors for now
  .long 0x900
  .quad 0x1 # from sector 1 (the second sector)
.org 446, 0x0
# MBR
# Active
.byte 0x80
# CHS Start Pos (NULL in miros)
.byte 0
.byte 0
.byte 0
# Partition Type
.byte 0x0C # FAT32 with LBA
# CHS End Pos (NULL in miros)
.byte 0
.byte 0
.byte 0
# LBA Start Pos
.long 32  # Boot System needs 32 sectors
# Sectors count
.long 98272  # 98304 - 32
.org 510, 0x0
.byte 0x55 # Boot Flags
.byte 0xaa
