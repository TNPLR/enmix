.PHONY: all clean
all: kernel.elf
kernel.elf: kernel.o
	${LD} ${LDFLAGS} -T init.ld -o $@ $^
kernel.o: pmain.c
	${CC} ${CFLAGS} -c -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
