extern irq_handler
irq_common:
        cli
	      pusha           ;push edi,esi,ebp,esp,ebx,edx,ecx,eax
        mov ax, ds
        push eax
        mov ax, 0x10    ;kernel data segment to all seg reg
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        push esp
        call irq_handler
        add esp, 4

        pop ebx
        mov ds, bx
        mov es, bx
        mov fs, bx
        mov gs, bx

        popa
        add esp, 8      ;Clean up pushed error code and isr number
        sti
        iret

; IRQ handlers.
global irq0
irq0:
  push byte 0
  push byte 32
  jmp irq_common

global irq1
irq1:
  push byte 0
  push byte 33
  jmp irq_common

global irq2
irq2:
  push byte 0
  push byte 34
  jmp irq_common

global irq3
irq3:
  push byte 0
  push byte 35
  jmp irq_common

global irq4
irq4:
  push byte 0
  push byte 36
  jmp irq_common

global irq5
irq5:
  push byte 0
  push byte 37
  jmp irq_common

global irq6
irq6:
  push byte 0
  push byte 38
  jmp irq_common

global irq7
irq7:
  push byte 0
  push byte 39
  jmp irq_common

global irq8
irq8:
  push byte 0
  push byte 40
  jmp irq_common

global irq9
irq9:
  push byte 0
  push byte 41
  jmp irq_common

global irq10
irq10:
  push byte 0
  push byte 42
  jmp irq_common

global irq11
irq11:
  push byte 0
  push byte 43
  jmp irq_common

global irq12
irq12:
  push byte 0
  push byte 44
  jmp irq_common

global irq13
irq13:
  push byte 0
  push byte 45
  jmp irq_common

global irq14
irq14:
  push byte 0
  push byte 46
  jmp irq_common

global irq15
irq15:
  push byte 0
  push byte 47
  jmp irq_common