.PHONY: all clean
all: kernel.elf
kernel.elf: kmain.o
	${LD} ${LDFLAGS} -T kern.ld -o $@ $^
kmain.o: kmain.c
	${CC} ${CFLAGS} -c -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
