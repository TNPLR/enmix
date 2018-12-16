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
  }
  .data :
  {
    *(.data)
  }
  .bss :
  {
    *(.bss)
  }
}
