.PHONY: all clean
all: kernel.elf
kernel.elf: kernel.o
	${LD} ${LDFLAGS} -Ttext 0x100000 -e pmain -o $@ $^
kernel.o: pmain.c
	${CC} ${CFLAGS} -c -o $@ $< -m32
	objcopy -I elf32-i386 -O elf64-x86-64 $@ $@
clean:
	rm -rf *.o *.bin *.elf
