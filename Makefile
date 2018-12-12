export CC=gcc
export CXX=g++
export AS=as
export LD=ld
export AR=ar
export NASM=nasm

export CFLAGS=
export CXXFLAGS=
export ASFLAGS=
export LDFLAGS=
export ARFLAGS=
export NASMFLAGS=

DEFAULT: all
.PHONY: start-test all clean img
start-test:
all: boot_all init_all
boot_all:
	${MAKE} all -C boot/
init_all:
	${MAKE} all -C init/
img:
	dd if=boot/mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc
	dd if=boot/rmain.bin of=./disk48M.hdd bs=512 seek=1 count=8 conv=notrunc
	dd if=./README.md of=./disk48M.hdd bs=512 seek=9 count=20 conv=notrunc
clean:
	${MAKE} clean -C boot/
	${MAKE} clean -C init/
