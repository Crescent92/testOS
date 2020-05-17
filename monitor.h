#ifndef MONITOR_H
#define MONITOR_H

#include "type.h"
#include "monitor.h"

void monitor_init();

void new_line();

void backspace();

//Clear screen
void monitor_clear();

void monitor_move_cursor();

void monitor_setcolor(uint8 color);

void monitor_putentryat(char c, uint8 color, uint32 x, uint32 y);

void monitor_putchar(char c);

void monitor_write(const char* data, uint32 size);

void monitor_writestring(const char* data);

int printf(const char *format, ...);
#endif

