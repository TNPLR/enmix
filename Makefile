CC = gcc
AS = as
LD = ld
OC = objcoby
TOPDIR := ${PWD}
CWARNFLAGS = -Wall -Wextra
CADDRFLAGS = -fno-plt -fno-pic
COPTFLAGS = -O2
CSTDFLAGS = -std=gnu11
CFLAGS = ${CWARNFLAGS} ${CADDRFLAGS} ${COPTFLAGS} ${CSTDFLAGS} \
				 -mno-red-zone -ffreestanding -mno-mmx -mno-sse -mno-sse2 \
				 -nostdlib -I${TOPDIR}/include/
ASFLAGS =
LDFLAGS =

BOOT_FILE =
INIT_FILE =
KERNEL_FILE =
DRIVERS_FILE =
LIB_FLIE =
SRC_BOOT = ${addprefix boot/, ${BOOT_FILE}}
SRC_INIT = ${addprefix boot/, ${INIT_FILE}}
SRC_KERNEL = ${addprefix boot/, ${KERNEL_FILE}}
SRC_DRIVERS = ${addprefix boot/, ${DRIVERS_FILE}}
SRC_LIB = ${addprefix boot/, ${LIB_FILE}}
OBJ_BOOT = ${SRC_BOOT: .S=.o .c=.o}
OBJ_INIT = ${SRC_INIT: .S=.o .c=.o}
OBJ_KERNEL = ${SRC_KERNEL: .S=.o .c=.o}
OBJ_DRIVERS = ${SRC_DRIVERS: .S=.o .c=.o}
OBJ_LIB = ${SRC_LIB: .S=.o .c=.o}
.DEFAULT_GOAL := all
all:
%.o: %.S
	${AS} ${ASFLAGS} -o $@ $<
%.o: %.c
	${CC} ${CFLAGS} -o $@ $<
