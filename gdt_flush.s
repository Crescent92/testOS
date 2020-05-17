
global gdt_flush
extern gdt_ptr
gdt_flush:
        ; Load gdt 
        lgdt [gdt_ptr]
        ; Move 0x10 to all segment register.
        ; 0x10 is offset in gdt to kernel data segment.
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ; Change code segment implicitly as it cannot
        ; be changed directly. 0x08 is index in gdt 
        ; to kernel code segment.
        jmp 0x08: flush
flush:
        ret
