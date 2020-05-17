#include "type.h"
#include "monitor.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "irq.h"

void kernel_main(void)
{
        monitor_init();
        printf("Initializing GDT!\n");
	init_gdt();

        printf("Initializing IDT!\n");
	init_idt();

//	asm volatile ("int $0x3");
//	asm volatile ("int $0x4");

	irq_install();

	init_timer(100);
}

