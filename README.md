# Miros - An Operation System for Fun
## Compile
```make clean```
to clean all the object files except hdd files

```make mrproper```
to clean all the object files include hdd files

```make all```
to compile all the object files, and build all the images

## Boot
When the system boot, there are many steps to do.
1. BIOS Start, and then read mbr.bin(boot sector) to 0x7c00
2. mbr.bin reads fat.bin from the second sector to 0x900
3. fat.bin reads loader.elf from the third sector to 0xB00

## Memory
