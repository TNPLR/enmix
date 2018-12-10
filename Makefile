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
all:
	cd boot; ${MAKE} all;
	cd init; ${MAKE} all
img:
	dd if=boot/mbr.bin of=./disk48M.hdd bs=512 count=1 conv=notrunc
	dd if=init/test.bin of=./disk48M.hdd bs=512 seek=1 count=8 conv=notrunc
clean:
	cd boot; ${MAKE} clean;
	cd init; ${MAKE} clean;
