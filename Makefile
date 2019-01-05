CC = gcc
AS = as
LD = ld
OC = objcopy
TOPDIR := ${PWD}
CWARNFLAGS = -Wall -Wextra
CADDRFLAGS = -fno-plt -fno-pic
COPTFLAGS = -O2
CSTDFLAGS = -std=gnu11
CFLAGS = ${CWARNFLAGS} ${CADDRFLAGS} ${COPTFLAGS} ${CSTDFLAGS} \
				 -mno-red-zone -ffreestanding -mno-mmx -mno-sse -mno-sse2 \
				 -nostdlib -I${TOPDIR}/include/
ASFLAGS =
LDFLAGS =

BOOT_FILE = mbr.S loader.S
INIT_FILE =
KERNEL_FILE =
DRIVERS_FILE =
LIB_FLIE =
SRC_BOOT = ${addprefix boot/, ${BOOT_FILE}}
SRC_INIT = ${addprefix boot/, ${INIT_FILE}}
SRC_KERNEL = ${addprefix boot/, ${KERNEL_FILE}}
SRC_DRIVERS = ${addprefix boot/, ${DRIVERS_FILE}}
SRC_LIB = ${addprefix boot/, ${LIB_FILE}}
OBJ_BOOT = ${SRC_BOOT:.S=.o}
OBJ_INIT = ${SRC_INIT:.S=.o .c=.o}
OBJ_KERNEL = ${SRC_KERNEL:.S=.o .c=.o}
OBJ_DRIVERS = ${SRC_DRIVERS:.S=.o .c=.o}
OBJ_LIB = ${SRC_LIB:.S=.o .c=.o}
.DEFAULT_GOAL := all
all: img
img: disk48M.hdd mbr.bin loader.bin
	dd if=mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc
	dd if=loader.bin of=./disk48M.hdd bs=512 seek=1 count=3 conv=notrunc
disk48M.hdd:
	dd if=/dev/zero of=./disk48M.hdd bs=512 count=98304
mbr.bin: ${OBJ_BOOT}
	${LD} ${LDFLAGS} -T boot/mbr.ld boot/mbr.o -o $@
	${OC} -I elf64-x86-64 -O binary $@ $@
loader.bin: ${OBJ_BOOT}
	${LD} ${LDFLAGS} -T boot/loader.ld boot/loader.o -o $@
	${OC} -I elf64-x86-64 -O binary $@ $@
%.o: %.S
	${AS} ${ASFLAGS} -o $@ $<
%.o: %.c
	${CC} ${CFLAGS} -o $@ $<
test:
	bochs -f ../bochs.bx
clean:
	rm -rf *.bin *.o boot/*.o
