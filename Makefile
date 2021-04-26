CC = i686-elf-gcc
O_LEVEL = 2
ROOTDIR = $(shell pwd)
C_FILES = $(wildcard \
kernel/*.c \
kernel/cpu/*.c \
kernel/devices/*.c \
kernel/fs/*.c \
kernel/locking/*.c \
kernel/mem/*.c \
kernel/proc/*.c \
kernel/system/*.c)

HEADERS = $(wildcard \
kernel/*.h \
kernel/cpu/*.h \
kernel/devices/*.h \
kernel/fs/*.h \
kernel/locking/*.h \
kernel/mem/*.h \
kernel/proc/*.h \
kernel/system/*.h)

ASM_FILES = $(wildcard \
kernel/*.s \
kernel/cpu/*.s \
kernel/devices/*.s \
kernel/fs/*.s \
kernel/locking/*.s \
kernel/mem/*.s \
kernel/proc/*.s \
kernel/system/*.s)

OBJS = ${C_FILES:.c=.o} ${ASM_FILES:.s=.o}

CFLAGS = -m32 -nostdlib -nostdinc -ffreestanding -fno-stack-protector \
	 -Wall -Wextra -ggdb -I$(ROOTDIR)/kernel -I$(ROOTDIR)/libc/include -c 

QEMUFLAGS = -m 1G -cdrom myos.iso

LDFLAGS = -melf_i386 -nostdlib -O${O_LEVEL} -T

myos.iso: myos.bin
	cp myos.bin isodir/boot/
	grub-mkrescue isodir -o myos.iso

myos.bin: ${OBJS} libc/libc.a
	i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib $^

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) $< -o $@ -lgcc

%.o: %.s
	nasm -gdwarf -f elf $< -o $@

.PHONY: run
run: myos.iso
	qemu-system-i386 ${QEMUFLAGS}
.PHONY: bochs
bochs: myos.iso
	bochs -f bochs.config -q

.PHONY: clean
clean:
	rm -f *.o
	find . -type f -name '*.o' -delete
	rm -rf isodir/boot/myos.bin myos.iso
