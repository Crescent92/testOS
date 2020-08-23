global get_esp
get_esp:
    mov eax, esp
    ret

global task_switch
extern current_task
task_switch:
    ;save process state, i.e, ebx, esi, edi and ebp.
    push ebx
    push esi
    push edi
    push ebp

    ;get previous task's control block
    mov edi, [current_task]
    mov [edi + 4], esp

    ;get next task from stack
    mov esi, [esp + (4+1)*4] ;This will get us the arg
    mov [current_task], esi

    ;restore next task's register and esp
    mov esp, [esi + 4]

    ; pop register of new task
    pop ebp
    pop edi
    pop esi
    pop ebx

    ;Pops eip from the stack.
    ret

global switch_usr_mode
extern task_user
switch_usr_mode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push long 0x23
    push eax
    pushf
    pop eax
    or eax, 0x200
    push eax
    push long 0x1B
    push cont
    iret
cont:
    ret