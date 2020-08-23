
global idt_flush
extern idt_ptr;
idt_flush:
	lidt [idt_ptr]
	ret

%macro ISR_NOERR 1
	global isr%1
	isr%1:
		cli
		push byte 0
		push byte %1
		jmp isr_common_stub
		ret
%endmacro


%macro ISR_ERR 1
	global isr%1
	isr%1:
		cli
		push byte %1
		jmp isr_common_stub
		ret
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8 
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 100
ISR_NOERR 128

global isr_common_stub
extern isr_handler
extern irq_handler
isr_common_stub:
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

global enable_interrupts
enable_interrupts:
	sti
	ret

global clear_interrupts
clear_interrupts:
	cli
	ret	
