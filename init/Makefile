.PHONY: all clean
all: kernel.bin
kernel.bin: rmain.o
	${LD} -T init.ld -o tmp.elf $^
	objcopy tmp.elf $@ -O binary
rmain.o: ./rmain.S
	${NASM} -f elf64 -o $@ $<
rprint.o: ./rprint.S
	${NASM} -f elf32 -o $@ $<
test.bin: test.S
	${NASM} -o $@ $<
clean:
	rm -rf *.o *.bin *.elf
