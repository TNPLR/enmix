export CC=gcc
export CXX=g++
export AS=as
export LD=ld
export AR=ar
export NASM=nasm
export OC=objcopy
export TOP_DIR := ${PWD}
export CFLAGS=-ffreestanding -fno-stack-protector \
							-mno-red-zone -Wall \
							-mno-mmx -mno-sse -mno-sse2 -nostdlib \
							-fno-plt -fno-pic -Wextra -I${TOP_DIR}/include/
export CXXFLAGS=
export ASFLAGS=
export LDFLAGS=
export ARFLAGS=
export NASMFLAGS=

DEFAULT: all
.PHONY: start-test all clean img test
start-test:
all: boot_all init_all kernel_all
boot_all:
	${MAKE} all -C boot/
init_all:
	${MAKE} all -C init/
kernel_all:
	${MAKE} all -C kernel/
img:
	dd if=boot/mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc
	dd if=boot/rmain.bin of=./disk48M.hdd bs=512 seek=1 count=8 conv=notrunc
	dd if=init/init.elf of=./disk48M.hdd bs=512 seek=9 count=32 conv=notrunc
	dd if=kernel/kernel.elf of=./disk48M.hdd bs=512 seek=41 count=4096 conv=notrunc
clean:
	${MAKE} clean -C boot/
	${MAKE} clean -C init/
	${MAKE} clean -C kernel/
test:
	bochs -f bochs.bx
