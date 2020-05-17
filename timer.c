

#include "timer.h"


uint32 tick = 0;

static void timer_callback(registers_t* regs) {
	tick++;
	printf("Tick: %d\n",tick);
}


void init_timer(uint32 freq) {
	printf("init_timer\n");
	irq_install_handler(IRQ0, &timer_callback);


	uint32 divisor = (1193180 / freq);

	outb(0x43, 0x36);

	uint8 l = (uint8)(divisor & 0xff);
	uint8 h = (uint8)((divisor >> 8) & 0xff);

	outb(0x40, l);
	outb(0x40, h);
}
