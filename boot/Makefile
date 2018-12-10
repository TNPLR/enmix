.PHONY: all clean
all: mbr.bin
mbr.bin: ./mbr.S
	${NASM} -o $@ $<
clean:
	rm -rf *.o *.bin
