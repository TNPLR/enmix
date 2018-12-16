.PHONY: all clean
all: init.elf
init.elf: init.o
	${LD} ${LDFLAGS} -T init.ld -o $@ $^
init.o: pmain.c
	${CC} ${CFLAGS} -c -o $@ $< -m32
	${OC} -I elf32-i386 -O elf64-x86-64 $@ $@
clean:
	rm -rf *.o *.bin *.elf
