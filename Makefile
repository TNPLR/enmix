# Variable in the program
CC = gcc
CXX = g++
AS = as
LD = ld
AR = ar
NASM = nasm
OC = objcopy
TOPDIR := ${PWD}
CWARNFLAGS= -Wall -Wextra -Wpedantic
CADDRFLAGS= -fno-plt -fno-pic
CFLAGS = ${CWARNFLAGS} -ffreestanding -fno-stack-protector \
							-mno-red-zone -std=gnu11 \
							-mno-mmx -mno-sse -mno-sse2 -nostdlib \
							-I${TOPDIR}/include/ ${CADDRFLAGS}
CXXFLAGS =
ASFLAGS =
LDFLAGS =
ARFLAGS =
NASMFLAGS =

SRC_BOOT = ${addprefix boot/,mbr.asm rmain.asm}
SRC_INIT = ${addprefix init/,pmain.c}
SRC_KERNEL_C = ${addprefix kernel/,interrupt.c kio.c \
	kmain.c e820.c memory.c thread.c assert.c}
SRC_KERNEL_ASM = ${addprefix kernel/,intr.asm switch.asm}
SRC_DRIVERS_C = ${addprefix drivers/,vga.c pic.c timer8253.c}
SRC_LIB_C = ${addprefix lib/,string.c bitmap.c deque.c}
OBJ_BOOT = ${SRC_BOOT:.asm=.bin}
OBJ_INIT = ${SRC_INIT:.c=.o}
OBJ_KERNEL_C = ${SRC_KERNEL_C:.c=.o} ${SRC_DRIVERS_C:.c=.o} ${SRC_LIB_C:.c=.o}
OBJ_KERNEL_ASM = ${SRC_KERNEL_ASM:.asm=.o}

.DEFAULT: DEFAULT
DEFAULT: all
.PHONY: start-test all clean img test mrproper
start-test:
kernel.elf: CFLAGS+=-mcmodel=large
kernel.elf: ${OBJ_KERNEL_C} ${OBJ_KERNEL_ASM}
	${CC} ${CFLAGS} -T kernel/kern.ld -o $@ $^
init.elf: CFLAGS += -m32
init.elf: ${OBJ_INIT}
	${LD} ${LDFLAGS} -T init/init.ld -o $@ ${OBJ_INIT}
init/pmain.o: init/pmain.c
	${CC} ${CFLAGS} -c -o $@ $<
	${OC} -I elf32-i386 -O elf64-x86-64 $@ $@
%.o: %.asm
	${NASM} ${NASMFLAGS} -f elf64 -o $@ $<
%.bin: %.asm
	${NASM} ${NASMFLAGS} -o $@ $< -i boot/
all: kernel.elf init.elf ${OBJ_BOOT}
	dd if=/dev/zero of=./disk48M.hdd bs=512 count=98304
	dd if=boot/mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc,sparse
	dd if=boot/rmain.bin of=./disk48M.hdd bs=512 seek=1 conv=notrunc,sparse
	dd if=init.elf of=./disk48M.hdd bs=512 seek=9 conv=notrunc,sparse
	dd if=kernel.elf of=./disk48M.hdd bs=512 seek=41 conv=notrunc,sparse
clean: RM_TARGETS = ${addsuffix *.bin,boot/ init/ kernel/ drivers/ lib/}\
										${addsuffix *.o,boot/ init/ kernel/ drivers/ lib/}
clean:
	rm -rf ${RM_TARGETS} *.elf
mrproper:
	rm -rf ${RM_TARGETS} *.elf *.hdd
test:
	bochs -f bochs.bx
