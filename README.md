# Miros - An Operation System for Fun
## Compile
'''make clean'''
to clean all the object files

'''make all'''
to compile all the object files

'''make img'''
to copy all the file into the disk image
## Boot
When the system boot, there are many steps to do.
1. BIOS Start, and then read mbr.bin(boot sector) to 0x7c00
2. mbr.bin using interrupt 0x13 to read loader from second sector to 0x900
3. second stage loader read kernel(2MB 4096 Sectors) to 0x400000 (4MB position)
4. read all the message to registers than jump to kernel
5. set up long-mode in kernel
