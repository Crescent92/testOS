#include "syscall.h"
#include "isr.h"
#include "monitor.h"
#include "irq.h"

#define SYSCALL_IRQ 0x80

static void *syscalls[3] = {
    &printf
};

uint32 num_syscalls = 3;

void syscall_handlers(registers_t *regs) {

}

void init_syscalls() {
    irq_install_handler(SYSCALL_IRQ, &syscall_handlers);
}