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
2. mbr.bin using interrupt 0x13 to read loader from second sector to 0x900
3. second stage loader read kernel(2MB 4096 Sectors) to 0x400000 (4MB position)
4. read all the message to registers than jump to kernel
5. check for longmode
6. set up paging at 0x1000

## Memory
0x200 ARDS Memory structure  
0x900 Real mode loader
0x910 GDT Table
0x9C0 TSS Structure
0x1000 ~ 0x9000 paging  
0x10000 init program  
0x100000 ~ 0x200000 kernel (0xc0100000 vaddr)  
0x204000 kernel stack (0xc0204000 vaddr)  
0x205000 double fault stack (0xc0208000)
0x208000 ~ 0x20C000 memory pool
