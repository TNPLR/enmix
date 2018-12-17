CFLAGS+=-mcmodel=large
.PHONY: all clean
all: kernel.elf
kernel.elf: kmain.o kio.o vga.o
	${LD} ${LDFLAGS} -T kern.ld -o $@ $^
kmain.o: kmain.c
	${CC} ${CFLAGS} -c -o $@ $<
kio.o: kio.c
	${CC} ${CFLAGS} -c -o $@ $<
vga.o: vga.c
	${CC} ${CFLAGS} -c -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
