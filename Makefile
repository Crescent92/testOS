
C_FILES = $(shell find . -type f -name '*.c')
HEADERS = $(shell find . -type f -name '*.h')
ASM_FILES = $(shell find . -type f -name '*.s')
OBJS = ${C_FILES:.c=.o} ${ASM_FILES:.s=.o}

CC = i686-elf-gcc
O_LEVEL = 2

CFLAGS = -m32 -nostdlib -nostdinc -ffreestanding -fno-stack-protector \
	 -Wall -Wextra -ggdb -c 

QEMUFLAGS = -cdrom myos.iso

LDFLAGS = -melf_i386 -nostdlib -O${O_LEVEL} -T

myos.iso: myos.bin
	cp myos.bin isodir/boot/
	grub-mkrescue isodir -o myos.iso

myos.bin: ${OBJS}
	ld ${LDFLAGS} linker.ld -o myos.bin $^

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	nasm -f elf $< -o $@

run: myos.iso
	qemu-system-i386 ${QEMUFLAGS}

bochs: myos.iso
	bochs -f bochs.config -q

.PHONY: clean
clean:
	rm -f *.o
	rm -rf isodir/boot/myos.bin myos.iso
