
global idt_flush
extern idt_ptr;
idt_flush:
	lidt [idt_ptr]
	ret

; 0: Divide by zero exception
global isr0
isr0:
	push byte 0
	push byte 0
	jmp isr_common_stub

; 1: Debug exception
global isr1
isr1:
	push byte 0
	push byte 1
	jmp isr_common_stub

; 2: NMI Exception
global isr2
isr2:
	push byte 0
	push byte 2
	jmp isr_common_stub

; 3: Breakpoint or INT3 exception.
global isr3
isr3:
	push byte 0
	push byte 3
	jmp isr_common_stub

; 4: Overflow or INTO Exception
global isr4
isr4:
	push byte 0
	push byte 4
	jmp isr_common_stub

; 5: Bound range exceeded(OOB) or BOUND exception.
global isr5
isr5:
	push byte 0
	push byte 5
	jmp isr_common_stub

; 6: Invalid opcode exception(UD2)
global isr6
isr6:
	push byte 0
	push byte 6
	jmp isr_common_stub

; 7: Device not available.
global isr7
isr7:
	push byte 0
	push byte 7
	jmp isr_common_stub

; 8: Double Fault
global isr8
isr8:
	push byte 8
	jmp isr_common_stub

; 9: Coprocessor segment overrun.
global isr9
isr9:
	push byte 0
	push byte 9
	jmp isr_common_stub

; 10: Invalid TSS.
global isr10
isr10:
	push byte 10
	jmp isr_common_stub

; 11: Segment not present.
global isr11
isr11:
	push byte 11
	jmp isr_common_stub

; 12: Stack-segment fault.
global isr12
isr12:
	push byte 12
	jmp isr_common_stub

; 13: General protection fault.
global isr13
isr13:
	push byte 13
	jmp isr_common_stub

; 14: Page fault exception.
global isr14
isr14:
	push byte 14
	jmp isr_common_stub

; 15: Reserved exception.
global isr15
isr15:
	push byte 0
	push byte 15
	jmp isr_common_stub

; 16: Floating point exception.
global isr16
isr16:
	push byte 0
	push byte 16
	jmp isr_common_stub

; 17: Alignment check exception.
global isr17
isr17:
	push byte 0
	push byte 17
	jmp isr_common_stub

; 18: Machine check exception.
global isr18
isr18:
	push byte 0
	push byte 18
	jmp isr_common_stub

; 19: SIMD Floating point exception.
global isr19
isr19:
	push byte 0
	push byte 19
	jmp isr_common_stub

; 20 - 31 are reserved.
; 32 - 255 are user definable.
; 20: Reserved. 
global isr20
isr20:
	push byte 0
	push byte 20
	jmp isr_common_stub

; 21: Reserved.
global isr21
isr21:
	push byte 0
	push byte 21
	jmp isr_common_stub

; 22: Reserved.
global isr22
isr22:
	push byte 0
	push byte 22
	jmp isr_common_stub

; 23: Reserved.
global isr23
isr23:
	push byte 0
	push byte 23
	jmp isr_common_stub

; 24: Reserved.
global isr24
isr24:
	push byte 0
	push byte 24
	jmp isr_common_stub

; 25: Reserved.
global isr25
isr25:
	push byte 0
	push byte 25
	jmp isr_common_stub

; 26: Reserved.
global isr26
isr26:
	push byte 0
	push byte 26
	jmp isr_common_stub

; 27: Reserved.
global isr27
isr27:
	push byte 0
	push byte 27
	jmp isr_common_stub

; 28: Reserved.
global isr28
isr28:
	push byte 0
	push byte 28
	jmp isr_common_stub

; 29: Reserved.
global isr29
isr29:
	push byte 0
	push byte 29
	jmp isr_common_stub

; Reserved.
global isr30
isr30:
	push byte 0
	push byte 30
	jmp isr_common_stub

; 31: Reserved.
global isr31
isr31:
	push byte 0
	push byte 31
	jmp isr_common_stub

; 128: System calls
global isr128
	isr128:
		push byte 0
		push 128
		jmp isr_common_stub



global isr_common_stub
extern isr_handler
extern irq_handler
isr_common_stub:
	cli
	pusha		;push edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds
	push eax
	mov ax, 0x10	;kernel data segment to all seg reg
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
	add esp, 8	;Clean up pushed error code and isr number
	sti
	iret

global irq_ditch
irq_ditch:
	iret

global enable_interrupts
enable_interrupts:
	sti
	ret

global clear_interrupts
clear_interrupts:
	cli
	ret	
