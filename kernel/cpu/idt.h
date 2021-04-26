
#ifndef _IDT_H
#define _IDT_H
#include <type.h>

struct idt_entry_struct {
        uint16 base_low;
        uint16 selector;
        uint8  always_zero;
        unsigned  gate_type : 4;
        unsigned  s : 1;
        unsigned  dpl : 2 ;
        unsigned  p : 1;
        uint16 base_high;
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct {
        uint16 limit;
        idt_entry_t * base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

void init_idt();

void idt_set_gate(int num, uint32 base, uint16 sel, uint8 g_type);

//ISRs
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10 ();
extern void isr11 ();
extern void isr12 ();
extern void isr13 ();
extern void isr14 ();
extern void isr15 ();
extern void isr16 ();
extern void isr17 ();
extern void isr18 ();
extern void isr19 ();
extern void isr20 ();
extern void isr21 ();
extern void isr22 ();
extern void isr23 ();
extern void isr24 ();
extern void isr25 ();
extern void isr26 ();
extern void isr27 ();
extern void isr28 ();
extern void isr29 ();
extern void isr30 ();
extern void isr31 ();
extern void isr128 ();
#endif

