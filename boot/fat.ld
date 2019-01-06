ENTRY(_start);
SECTIONS {
  . = 0x900;
  .text : 
  {
    *(.fat)
    *(.text)
  }
}
