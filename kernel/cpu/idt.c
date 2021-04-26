#include "idt.h"
#include <type.h>
#include <common.h>
#include <devices/io.h>
#include <cpu/irq.h>

extern void idt_flush();
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;
/*
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
*/

extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

extern void irq_ditch();

extern void enable_interrupts();

void idt_set_gate(int num, uint32 base, uint16 sel, uint8 g_type) {
        idt_entries[num].base_low    = (base & 0xffff);
        idt_entries[num].base_high   = (base >> 16) & 0xffff;
        idt_entries[num].selector    = sel;
        idt_entries[num].gate_type = g_type;
        idt_entries[num].s = 0;
        idt_entries[num].dpl = 3;
        idt_entries[num].p = 1;
        idt_entries[num].always_zero = 0;
}


void init_idt() {
        idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
        idt_ptr.base  = idt_entries;

        memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

        //0x8e = segment present, dpl = 0, d = 1 for 32 bit 
        idt_set_gate(0, (uint32)isr0, 0x08, 0xE);
        idt_set_gate(1, (uint32)isr1, 0x08, 0xE);
        idt_set_gate(2, (uint32)isr2, 0x08, 0xE);
        idt_set_gate(3, (uint32)isr3, 0x08, 0xE);
        idt_set_gate(4, (uint32)isr4, 0x08, 0xE);
        idt_set_gate(5, (uint32)isr5, 0x08, 0xE);
        idt_set_gate(6, (uint32)isr6, 0x08, 0xE);
        idt_set_gate(7, (uint32)isr7, 0x08, 0xE);
        idt_set_gate(8, (uint32)isr8, 0x08, 0xE);
        idt_set_gate(9, (uint32)isr9, 0x08, 0xE);
        idt_set_gate(10, (uint32)isr10, 0x08, 0xE);
        idt_set_gate(11, (uint32)isr11, 0x08, 0xE);
        idt_set_gate(12, (uint32)isr12, 0x08, 0xE);
        idt_set_gate(13, (uint32)isr13, 0x08, 0xE);
        idt_set_gate(14, (uint32)isr14, 0x08, 0xE);
        idt_set_gate(15, (uint32)isr15, 0x08, 0xE);
        idt_set_gate(16, (uint32)isr16, 0x08, 0xE);
        idt_set_gate(17, (uint32)isr17, 0x08, 0xE);
        idt_set_gate(18, (uint32)isr18, 0x08, 0xE);
        idt_set_gate(19, (uint32)isr19, 0x08, 0xE);
        idt_set_gate(20, (uint32)isr20, 0x08, 0xE);
        idt_set_gate(21, (uint32)isr21, 0x08, 0xE);
        idt_set_gate(22, (uint32)isr22, 0x08, 0xE);
        idt_set_gate(23, (uint32)isr23, 0x08, 0xE);
        idt_set_gate(24, (uint32)isr24, 0x08, 0xE);
        idt_set_gate(25, (uint32)isr25, 0x08, 0xE);
        idt_set_gate(26, (uint32)isr26, 0x08, 0xE);
        idt_set_gate(27, (uint32)isr27, 0x08, 0xE);
        idt_set_gate(28, (uint32)isr28, 0x08, 0xE);
        idt_set_gate(29, (uint32)isr29, 0x08, 0xE);
        idt_set_gate(30, (uint32)isr30, 0x08, 0xE);
        idt_set_gate(31, (uint32)isr31, 0x08, 0xE);
        idt_set_gate(128, (uint32)isr128, 0x08, 0xE);

        
	idt_set_gate(0x81, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x82, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x83, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x84, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x85, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x86, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x87, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x88, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x89, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8A, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8B, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8C, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8D, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8E, (uint32)irq_ditch, 0x08, 0x8E);
	idt_set_gate(0x8F, (uint32)irq_ditch, 0x08, 0x8E);

	idt_flush();

}

