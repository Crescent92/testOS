#include <common.h>
#include "isr.h"
#include <devices/monitor.h>

void isr_handler(registers_t * regs) {
        printf("Received interrupt: %d\n", regs->int_no);
}

