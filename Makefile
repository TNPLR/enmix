# Variable in the program
CC = gcc
CXX = g++
AS = as
LD = ld
AR = ar
NASM = nasm
OC = objcopy
TOPDIR := ${PWD}
CFLAGS = -ffreestanding -fno-stack-protector \
							-mno-red-zone -Wall \
							-mno-mmx -mno-sse -mno-sse2 -nostdlib \
							-fno-plt -fno-pic -Wextra -I${TOPDIR}/include/
CXXFLAGS =
ASFLAGS =
LDFLAGS =
ARFLAGS =
NASMFLAGS =

SRC_BOOT = ${addprefix boot/,mbr.asm rmain.asm}
SRC_INIT = ${addprefix init/,pmain.c}
SRC_KERNEL_C = ${addprefix kernel/,interrupt.c kio.c kmain.c}
SRC_KERNEL_ASM = ${addprefix kernel/,intr.asm}
SRC_DRIVERS_C = ${addprefix drivers/,vga.c pic.c timer8253.c}
OBJ_BOOT = ${SRC_BOOT:.asm=.bin}
OBJ_INIT = ${SRC_INIT:.c=.o}
OBJ_KERNEL_C = ${SRC_KERNEL_C:.c=.o} ${SRC_DRIVERS_C:.c=.o}
OBJ_KERNEL_ASM = ${SRC_KERNEL_ASM:.asm=.o}

.DEFAULT: DEFAULT
DEFAULT: all
.PHONY: start-test all clean img test
start-test:
all: img
kernel.elf: CFLAGS+=-mcmodel=large
kernel.elf: ${OBJ_KERNEL_C} ${OBJ_KERNEL_ASM}
	${LD} ${LDFLAGS} -T kernel/kern.ld -o $@ $^
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
img: kernel.elf init.elf ${OBJ_BOOT}
	dd if=boot/mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc
	dd if=boot/rmain.bin of=./disk48M.hdd bs=512 seek=1 count=8 conv=notrunc
	dd if=init.elf of=./disk48M.hdd bs=512 seek=9 count=32 conv=notrunc
	dd if=kernel.elf of=./disk48M.hdd bs=512 seek=41 count=4096 conv=notrunc
clean: RM_TARGETS = ${addsuffix *.bin,boot/ init/ kernel/ drivers/}\
										${addsuffix *.o,boot/ init/ kernel/ drivers/}
clean:
	rm -rf ${RM_TARGETS} *.elf
test:
	bochs -f bochs.bx
