CC = gcc
AS = as
LD = ld
OC = objcopy
TOPDIR := ${PWD}
CWARNFLAGS = -Wall -Wextra
CADDRFLAGS = -fno-plt -fno-pic
COPTFLAGS = -O0
CSTDFLAGS = -std=gnu11
CFLAGS = ${CWARNFLAGS} ${CADDRFLAGS} ${COPTFLAGS} ${CSTDFLAGS} \
				 -mno-red-zone -ffreestanding -mno-mmx -mno-sse -mno-sse2 \
				 -nostdlib -I${TOPDIR} -static
ASFLAGS =
LDFLAGS =

.PHONY: all clean iso
BOOT_FILE = mbr.S boot16.S ext2.S
INIT_FILE_S = boot32.S multiboot.S
INIT_FILE_C = pmain.c
ARCH_FILE_C =
KERNEL_FILE_C =
DRIVERS_FILE =
LIB_FLIE =
SRC_BOOT = ${addprefix boot/, ${BOOT_FILE}}
SRC_INIT_S = ${addprefix init/, ${INIT_FILE_S}}
SRC_INIT_C = ${addprefix init/, ${INIT_FILE_C}}
SRC_ARCH_C = ${addprefix arch/, ${ARCH_FILE_C}}
SRC_KERNEL = ${addprefix boot/, ${KERNEL_FILE}}
SRC_DRIVERS = ${addprefix boot/, ${DRIVERS_FILE}}
SRC_LIB = ${addprefix boot/, ${LIB_FILE}}
OBJ_BOOT = ${SRC_BOOT:.S=.o}
OBJ_KERNEL = ${SRC_INIT_S:.S=.o} ${SRC_INIT_C:.c=.o} \
						 ${SRC_KERNEL_C:.c=.o} ${SRC_ARCH_C:.c=.o}
OBJ_DRIVERS = ${SRC_DRIVERS:.S=.o .c=.o}
OBJ_LIB = ${SRC_LIB:.S=.o .c=.o}
.DEFAULT_GOAL := all
all: img
img: disk48M.hdd mbr.bin ext2.bin
	dd if=mbr.bin of=./disk48M.hdd bs=512 count=2 conv=notrunc
	dd if=ext2.bin of=./disk48M.hdd bs=512 seek=1 count=32 conv=notrunc
disk48M.hdd:
	dd if=/dev/zero of=./disk48M.hdd bs=512 count=98304
mbr.bin: ${OBJ_BOOT}
	${LD} ${LDFLAGS} -T boot/mbr.ld boot/mbr.o -o $@
	${OC} -I elf64-x86-64 -O binary $@ $@
ext2.bin: ${OBJ_BOOT}
	${LD} ${LDFLAGS} -T boot/ext2.ld boot/ext2.o boot/boot16.o -o $@
	${OC} -I elf64-x86-64 -O binary $@ $@
kernel.bin: ${OBJ_KERNEL}
	${LD} ${LDFLAGS} -n -T kernel.ld $^ -o $@
%.o: %.S
	${AS} ${ASFLAGS} -o $@ $<
%.o: %.c
	${CC} ${CFLAGS} -o $@ $<
init/pmain.o: init/pmain.c
	${CC} ${CFLAGS} -c $< -o $@ -m32
	${OC} -I elf32-i386 -O elf64-x86-64 $@ $@
arch/direct_vga.o: arch/direct_vga.c
	${CC} ${CFLAGS} -c $< -o $@ -m32
	${OC} -I elf32-i386 -O elf64-x86-64 $@ $@
test: qemu-test
bochs-test:
	bochs -f ../bochs.bx
qemu-test:
	#qemu-system-x86_64 -drive file=disk48M.hdd,format=raw -enable-kvm
	qemu-system-x86_64 -cdrom os.iso -enable-kvm
clean:
	rm -rf *.bin *.o boot/*.o init/*.o os.iso arch/*.o
iso: os.iso
os.iso: kernel.bin
	mkdir iso/
	mkdir iso/boot/
	mkdir iso/boot/grub/
	cp kernel.bin iso/boot/
	cp defaultgrub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o os.iso iso/
	rm -rf iso/

