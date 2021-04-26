#ifndef _LIBC_PRINTF_H
#define _LIBC_PRINTF_H


#include <stdarg.h>
#include <type.h>

int printf(const char* fmt, ...);

int cprintf(const char *fmt, ...);

#endif