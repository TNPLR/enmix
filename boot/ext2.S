.code16
.section .ext2
.global _start
_start:
  jmp boot
boot:
# Use 0x13 interrupt to read 3 sectors (second stage loader)
# Clear registers
  xorw %ax, %ax
  xorw %dx, %dx
  xorw %si, %si

  movb $0x42, %ah   # Extended Read Sectors From Drive
  movb $0x80, %dl   # First Hard disk
  movw $DAC, %si
  int $0x13

  movw $0xB00, %ax
  jmp *%ax
DAC:
  .byte 0x10 # Sizeof DAC
  .byte 0 # unused
  .word 32 # Read 1 sectors (FAT Boot record)
  .long 0xB00
  .quad 0x2 # from sector 2 (the third sector)
.org 510, 0x0

# Boot flags
.byte 0x55
.byte 0xaa
# Additional boot record data
# Nothing now

.org 512, 0x0
# Superblock

# s_inodes_count
.long 0

# s_blocks_count 49151 blocks * 1024kiB per block = 49151kiB
.long 49151

# s_r_blocks_count
.long 0

# s_free_blocks_count
.long 0

# s_free_inodes_count
.long 0

# s_first_data_block
.long 0

# s_log_block_size
.long 0

# s_log_frag_size
.long 0

# s_blocks_per_group
.long 8192

# s_frags_per_group
.long 0

# s_inodes_per_group
.long 0

# s_mtime
.long 0

# s_wtime
.long 0

# s_mnt_count
.word 0

# s_max_mnt_count
.word 0

# s_magic
.word 0

# s_state
.word 0

# s_errors
.word 0

# s_minor_rev_level
.word 0

# s_lastcheck
.long 0

# s_checkinterval
.long 0

# s_creator_os
.long 0

# s_rev_level
.long 0

# s_def_resuid
.word 0

# s_def_resgid
.word 0


