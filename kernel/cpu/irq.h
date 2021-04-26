#ifndef _IRQ_H
#define _IRQ_H


#include <type.h>
#include "isr.h"

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
typedef void (*isr_t)(registers_t *);

void irq_install_handler (uint32 irq, isr_t  handler);

void irq_uninstall_handler (uint32 irq);

void pic_remap();

void irq_gates();

void irq_install();

void irq_ack(uint32 irq_no);

void irq_handler(registers_t *r);

#endif
