.PHONY: all clean
all: mbr.bin rmain.bin
mbr.bin: ./mbr.S
	${NASM} -o $@ $<
rmain.bin: ./rmain.S
	${NASM} -o $@ $<
clean:
	rm -rf *.o *.bin
