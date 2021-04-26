
global asm_syscall
asm_syscall:

    push ebp
    mov ebp, esp
    ;push register as they contain args
    mov edi, [esp + 28] 
    mov esi, [esp + 24]
    mov edx, [esp + 20]
    mov ecx, [esp + 16]
    mov ebx, [esp + 12]
    mov eax, [esp + 8]
    
    int 0x80
    pop ebp


    ret