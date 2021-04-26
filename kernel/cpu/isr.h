#ifndef _ISR_H
#define _ISR_H

#include <common.h>

typedef struct reg{
  // Data segment selector
  uint32 ds;
  // Pushed by pusha.
  uint32 edi,esi,ebp,useless_value,ebx,edx,ecx,eax;
  // Interrupt number and error code.
  uint32 int_no,err_code;
  // Pushed by the processor automatically.
  uint32 eip,cs,eflags,esp,ss;
} __attribute__((packed)) registers_t;

#endif
