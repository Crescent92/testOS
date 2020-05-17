#include "idt.h"
#include "type.h"
#include "common.h"
#include "irq.h"

extern void idt_flush();
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void idt_set_gate(int num, uint32 base, uint16 sel, uint8 g_type) {
        idt_entries[num].base_low    = (base & 0xffff);
        idt_entries[num].base_high   = (base >> 16) & 0xffff;
        idt_entries[num].selector    = sel;
        idt_entries[num].gate_type = g_type;
        idt_entries[num].s = 0;
        idt_entries[num].dpl = 0;
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

	idt_flush();

}

