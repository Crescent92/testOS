global start

extern kernel_main

KERNEL_STACK_SIZE equ 4096

SECTION .text
start:
        mov esp, kernel_stack + KERNEL_STACK_SIZE
        call kernel_main
.loop:
        jmp .loop

SECTION .bss
align 4
kernel_stack:
        resb KERNEL_STACK_SIZE

