CFLAGS=-m32 -ggdb -std=gnu99 -I . -I../libc/include -Werror -ffreestanding -fno-exceptions
CC = i686-elf-gcc

C_FILES = task2.c test3.c

OBJS = ${C_FILES:.c=.o}
all: test test2
	cp test test2 binaries/

test: task2.o
	$(CC) -T link.ld -o $@ task2.o ../libc/libc.a -ffreestanding -O2 -nostdlib -lgcc

test2: test3.o
	$(CC) -T link.ld -o $@ test3.o ../libc/libc.a -ffreestanding -O2 -nostdlib -lgcc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f binaries/*
	rm -f *.o
	rm -f test
