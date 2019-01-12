ENTRY(_start);

SECTIONS {
  . = 1M;
  .boot :
  {
    *(.multiboot_header)
  }
  .text32 :
  {
    *(.text32)
    *(pmain.text32)
    _lmatext = .;
  }
}
