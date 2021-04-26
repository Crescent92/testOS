

#include "keyboard.h"
#include "keyboard_map.h"
#include <cpu/irq.h>


void init_keyboard() {
	irq_install_handler(IRQ1, &keyboard_handler);
}

void keyboard_handler(registers_t *r) {
	unsigned char scancode;

	scancode = inb(0x60);

	if (scancode & 0x80) {
	} else {
		printf("%c",us_keyboard_map[scancode]);
	}
}
