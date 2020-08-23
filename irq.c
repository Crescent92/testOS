
#include "irq.h"
#include "io.h"
#include "pic.h"
#include "idt.h"
#include "monitor.h"


extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

isr_t interrupt_handlers[256] = { NULL };

void irq_install_handler (uint32 irq, isr_t handler) {
	interrupt_handlers[irq] = handler;
}

void irq_uninstall_handler (uint32 irq) {
	interrupt_handlers[irq] = NULL;
}

void pic_remap() {
	outb(0x20, 0x11);	//Init PIC1 master
	outb(0xA0, 0x11);	//Init PIC2 slave
	outb(0x21, 0x20);	//Vector offeset master (32 decimal)
	outb(0xA1, 0x28);	//Vector offset slave (40 decimal)
	outb(0x21, 0x04);	//tell master that there's a slave at irq2(0000 0100)
	outb(0xA1, 0x02);	//tell slave pic it's slave/cascade identity
	outb(0x21, 0x01);	//8086/88 mode
	outb(0xA1, 0x01);	//8086/88 mode
	outb(0x21, 0x0);	//enable all IRQs on master
	outb(0xA1, 0x0);	//enable all IRQs on slave
}


void irq_gates() {
	idt_set_gate(32, (uint32)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32)irq15, 0x08, 0x8E);
}

void irq_install() {
	pic_remap();
	irq_gates();
	asm volatile ("sti");
}

#define IS_IRQ(int_no) ((int_no >= 32) && (int_no <=47))
void irq_handler(registers_t *r) {
	//printf("Interrupt: %d\n",r->int_no);
	if(IS_IRQ(r->int_no)) {
		if (r->int_no >= 40) {
			outb(PIC2, PIC_EOI);
		}
		outb(PIC1, PIC_EOI);
	}

	if (interrupt_handlers[r->int_no] != 0) {
//		printf("Executing irq_handler\n");
		isr_t handler = interrupt_handlers[r->int_no];
		handler(r);
	}

}

void check() {
	printf("Check here\n");
}
