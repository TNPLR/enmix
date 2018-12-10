.PHONY: all clean
all: test.bin
rkernel.bin: rmain.o rprint.o
rmain.o: ./rmain.c
	${CC} ${CFLAGS} -m16 -c -o $@ $<
rprint.o: ./rprint.S
	${NASM} -f elf32 -o $@ $<
test.bin: test.S
	${NASM} -o $@ $<
clean:
	rm -rf *.o *.bin
