CFLAGS+=-mcmodel=large
.PHONY: all clean
all: kernel.elf
kernel.elf: kmain.o kio.o vga.o interrupt.o asm_interrupt.o vga.o
	${LD} ${LDFLAGS} -T kern.ld -o $@ $^
kmain.o: kmain.c
	${CC} ${CFLAGS} -c -o $@ $<
kio.o: kio.c
	${CC} ${CFLAGS} -c -o $@ $<
interrupt.o: interrupt.c
	${CC} ${CFLAGS} -c -o $@ $<
asm_interrupt.o: interrupt.S
	${NASM} -o $@ $< -f elf64
vga.o: vga.c
	${CC} ${CFLAGS} -c -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
