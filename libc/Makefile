
ROOTDIR = $(shell pwd)
C_FILES = $(wildcard *.c **/*.c)
HEADERS = $(wildcard include/*.h)
ASM_FILES = $(wildcard *.s **/*.s)

OBJS = ${C_FILES:.c=.o} ${ASM_FILES:.s=.o}

CC = i386-testOS-gcc
AR = i386-testOS-ar
O_LEVEL = 2

CFLAGS = -m32 -ggdb -nostdlib -nostdinc -ffreestanding -fno-stack-protector \
	 -Wall -Wextra -ggdb -c -I$(ROOTDIR)/include

all: crt0.o libc.a

libc.a: ${OBJS}
	$(AR) rcs $@ $(OBJS)

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	nasm -gdwarf -f elf $< -o $@

crt0.o: crt0.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o **/*.o
	rm -f *.a
	find . -name "*.o" | xargs rm -f
