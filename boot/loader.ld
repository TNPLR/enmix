ENTRY(loader);
SECTIONS {
  . = 0x900;
  .text : 
  {
    *(.text)
  }
}
