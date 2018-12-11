ENTRY(loader_start);
SECTIONS {
  . = 0x900;
  .loader_start : { *(.loader_start) }
}
