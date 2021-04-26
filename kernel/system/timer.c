

#include "timer.h"
#include <proc/process.h>


uint32 time_in_ms_since_boot = 0;
extern void timer_proc_wait_task_cb();

static void timer_callback(registers_t* regs) {
	time_in_ms_since_boot++;
	// timer_proc_wait_task_cb();
	schedule();
	// printf("Tick: %d int: %d\n",time_in_ms_since_boot,regs->int_no);
}


void init_timer(uint32 freq) {
	printf("init_timer\n");
	irq_install_handler(32, &timer_callback);


	uint32 divisor = (1193180 / freq);

	outb(0x43, 0x36);

	uint8 l = (uint8)(divisor & 0xff);
	uint8 h = (uint8)((divisor >> 8) & 0xff);

	outb(0x40, l);
	outb(0x40, h);
}
