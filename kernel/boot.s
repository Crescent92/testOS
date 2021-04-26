
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

; Mulitiboot header, must be 32 bit aligned 
section .multiboot
align 4
        dd MAGIC
        dd FLAGS
        dd CHECKSUM


extern kernel_main

KERNEL_STACK_SIZE equ 4096

SECTION .text
global start
start:
        mov esp, kernel_stack + KERNEL_STACK_SIZE
	push ebx
        call kernel_main
.loop:
        jmp .loop

SECTION .bss
align 4
kernel_stack:
        resb KERNEL_STACK_SIZE
