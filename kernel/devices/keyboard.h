#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cpu/isr.h>
#include "io.h"
#include "monitor.h"
void init_keyboard();

void keyboard_handler(registers_t *r);

#endif // KEYBOARD_H

