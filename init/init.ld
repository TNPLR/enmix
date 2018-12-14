ENTRY(pmain)
SECTIONS {
  . = 1M;
  .text :
  {
    *(.text.pmain)
    *(.text)
  }
  .rodata :
  {
    *(.rodata)
    rodata_end = .;
  }
  .data : AT (rodata_end)
  {
    *(.data)
  }
  .bss :
  {
    *(.bss)
  }
}
