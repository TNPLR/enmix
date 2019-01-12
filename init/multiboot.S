.code32
.section .multiboot_header, "a"
multiboot_start:
  .long 0xe85250d6    # Magic Number
  .long 0x0           # Architecture 0 (i386)
  .long multiboot_end - multiboot_start  # Header Length

# Checksum
  .long 0x100000000 - (0xe85250d6 + 0 + (multiboot_end - multiboot_start))

# End tag
  .word 0
  .word 0
  .long 8
multiboot_end:
