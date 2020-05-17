#include "monitor.h"
#include "stddef.h"
#include "stdarg.h"
#include "stdint.h"
#include "type.h"
#include "common.h"
#include "io.h"

enum vga_color {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_MAGENTA = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_LIGHT_GREY = 7,
        VGA_COLOR_DARK_GREY = 8,
        VGA_COLOR_LIGHT_BLUE = 9,
        VGA_COLOR_LIGHT_GREEN = 10,
        VGA_COLOR_LIGHT_CYAN = 11,
        VGA_COLOR_LIGHT_RED = 12,
        VGA_COLOR_LIGHT_MAGENTA = 13,
        VGA_COLOR_LIGHT_BROWN = 14,
        VGA_COLOR_WHITE = 15,
};

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

//Frame buffer start address
uint16 * video_memory = (uint16 *)0xB8000;
uint32 monitor_row, monitor_column;
uint8 monitor_color;
uint16 * monitor_buffer = 0;

//Globals for cursor position
uint8 cursor_x = 0;
uint8 cursor_y = 0;

static const uint32 VGA_WIDTH = 80;
static const uint32 VGA_HEIGHT = 25;


static inline uint8 make_color(enum vga_color fg, enum vga_color bg)
{
        return (fg | (bg << 4));
}

static inline uint16 make_vga_entry(unsigned char uc, uint8 color)
{
        return (((uint16) uc) | ((uint16) color << 8));
}


/*
 * monitor_move_cursor:
 * Move cursor to given position.
 * @param pos: The new position of cursor
 */
void monitor_move_cursor() {
        uint32 temp = monitor_row*80 + monitor_column;
        outb(FB_COMMAND_PORT,   FB_HIGH_BYTE_COMMAND);
        outb(FB_DATA_PORT,      ((temp >> 8) & 0xFF));
        outb(FB_COMMAND_PORT,   FB_LOW_BYTE_COMMAND);
        outb(FB_DATA_PORT,      (temp & 0xFF));
}

void monitor_clear() {
        monitor_buffer = video_memory;
        for (uint32 y = 0; y < VGA_HEIGHT; y++) {
                for (uint32 x = 0; x < VGA_WIDTH; x++) {
                        const uint32 index = y * VGA_WIDTH + x;
                        monitor_buffer[index] = make_vga_entry(' ', monitor_color);
                }
        }

        monitor_row = 0;
        monitor_column = 0;

        monitor_move_cursor();
}

void new_line() {
	if (monitor_row >= VGA_HEIGHT) {
		monitor_row = 0;
		monitor_column = 0;
		monitor_clear();
	}
	monitor_column = 0;
	monitor_row++;
	monitor_move_cursor();
}

void backspace() {
	monitor_column -= 1;
	monitor_putentryat(' ', monitor_color, monitor_column, monitor_row);
	monitor_move_cursor();
}

void monitor_init()
{
        monitor_color = make_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        monitor_clear();
}

void monitor_setcolor(uint8 color)
{
        monitor_color = color;
}

void monitor_putentryat(char c, uint8 color, uint32 x, uint32 y)
{
        const uint32 index = y * VGA_WIDTH + x;
        monitor_buffer[index] = make_vga_entry(c, color);
}

void monitor_putchar(char c)
{
	if (c == '\n') {
		new_line();
	} else {
        	monitor_putentryat(c, monitor_color, monitor_column, monitor_row);
        	if (++monitor_column == VGA_WIDTH) {
                	monitor_column = 0;
                	if (++monitor_row == VGA_HEIGHT)
                        	monitor_row = 0;
        	}
        	monitor_move_cursor();
	}
}

void monitor_write(const char* data, uint32 size)
{
        for (uint32 i = 0; i < size; i++)
                monitor_putchar(data[i]);
}

void monitor_writestring(const char* data)
{
        monitor_write(data, strlen(data));
}

int printf(const char *format, ...){
  char buf[20],c,*s;
  int val;

  /* ap is the object of va_list and it will hold the
   * information needed to retrieve the additional
   * arguments with va_arg */
  va_list ap;

  va_start(ap,format);

  /* d,i => int; decimal number
   * x => int; unsigned hexadecimal number
   * c => int; single character
   * s => char *; print characters from the string until a '\0'
   * or the number of characters given by the precision.*/
  for(uint32 i=0; i < strlen(format); i++){
    if (format[i] == '%'){
      i++;
      switch(format[i]){
        case 'i':
          val = va_arg(ap, int);
          itoa(val, buf, 10);
          monitor_writestring(buf);
          break;
        case 'x':
          val = va_arg(ap, uint32);
          uitoa(val, buf, 16);
          monitor_writestring(buf);
          break;
        case 'd':
          val = va_arg(ap, uint32);
          uitoa(val, buf, 10);
          monitor_writestring(buf);
          break;
        case 'c':
          c = (char)va_arg(ap, int);
          monitor_putchar(c);
          break;
	case 's':
          s = va_arg(ap, char*);
          monitor_writestring(s);
          break;
        default:
          monitor_putchar(format[i]);
      }
    }
    else{
      monitor_putchar(format[i]);
    }
  }

  va_end(ap);
  return 0;
}
