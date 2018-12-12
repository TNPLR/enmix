.PHONY: all clean
all: kernel.elf
kernel.elf: kernel.o
	${LD} ${LDFLAGS} -Ttext 0x100000 -e main -o $@ $^
kernel.o: kinit.c
	${CC} ${CFLAGS} -c -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
