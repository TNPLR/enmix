.PHONY: all clean
all: kernel.elf
kernel.elf: kmain.o
	${LD} ${LDFLAGS} -T kern.ld -o $@ $^
kmain.o: kmain.c
	${CC} ${CFLAGS} -c -o $@ $< -m32
	${OC} -I elf32-i386 -O elf64-x86-64 $@ $@
clean:
	rm -rf *.o *.bin *.elf
