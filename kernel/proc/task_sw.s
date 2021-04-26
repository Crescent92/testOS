global get_esp
get_esp:
    mov eax, esp
    ret

global task_switch
extern current_task
task_switch:
    ;save process state, i.e, ebx, esi, edi and ebp.
  ;  cli
    push ebx
    push esi
    push edi
    push ebp

    ;get previous task's control block
    mov edi, [current_task]
    mov [edi + 8], esp

    ;get next task from stack
    mov esi, [esp + (4+1)*4] ;This will get us the arg
    mov [current_task], esi

    ;restore next task's register and esp
    mov esp, [esi + 8]
    mov edi, [esi + 28]
    mov cr3, edi

   
    ; pop register of new task
    pop ebp
    pop edi
    pop esi
    pop ebx

    ;Pops eip from the stack.
   ; sti
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
    pushfd
    pop eax
    or eax, 0x200
    push eax
    push long 0x1B
    push cont
    iret
cont:
    ret

global user_mode_ret
user_mode_ret:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, [esp + 4] ;get arg from stack

    push dword [eax + 15*4]     ;ss
    push dword [eax + 14*4]     ;esp 
    push dword [eax + 13*4]     ;eflags
    push dword [eax + 12*4]     ;cs
    push dword [eax + 11*4]     ;eip

    mov edi, [eax + 1*4]        ;edi
    mov esi, [eax + 2*4]        ;esi
    mov ebp, [eax + 3*4]        ;ebp
    mov ebx, [eax + 5*4]        ;ebx
    mov edx, [eax + 6*4]        ;edx
    mov ecx, [eax + 7*4]        ;ecx
    mov eax, [eax + 8*4]        ;eax

    sti
    iret
 
global start_user_mode_thread
start_user_mode_thread:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, [esp + 4]  ; addr of entry
    mov ebx, [esp + 8]  ; push user esp
    push long 0x23  ; ss
    push ebx        ; esp
    pushf           ; eflags
    pop ebx
    or ebx, 0x200
    push ebx
    push long 0x1B  ; user cs
    push eax
    iret    

