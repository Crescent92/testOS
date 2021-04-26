
#ifndef _FAULT_H
#define _FAULT_H

#include "type.h"
#include "monitor.h"

void panic_impl(const char* msg, const char* file, uint32 line);

#define KPANIC(msg) panic_impl(msg, __FILE__, __LINE__)

#define ASSERT(cond) ((cond) ?  (void)0 : panic_impl("Assert failed", __FILE__, __LINE__))

#endif
