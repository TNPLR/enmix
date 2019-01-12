ENTRY(_start);
SECTIONS {
  . = 0x900;
  .text : 
  {
    *(.ext2)
    *(.text)
  }
}
