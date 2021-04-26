#include "syscall.h"
#include "cpu/isr.h"
#include "devices/monitor.h"
#include "cpu/irq.h"
#include <proc/process.h>
#include <proc/signals.h>

#define SYSCALL_IRQ 0x80


typedef uint32 (*handler_t) (unsigned int, ...);
// static void sys_monitor_write_str(registers_t *regs) {
//     monitor_writestring(regs->ebx);     //address of string in userspace?
// }

//yield, sleep, waitpid,
//signal,  sigaction, sigsuspend, sigreturn, sigprocmask,

void _sys_write (char *buf, int sz) {
    // printf("Oh! Hello!\n");
    monitor_write(buf, sz);
} 

void _sys_exit (int code) {
    proc_exit();
    return;
}

int _sys_fork() {
    return fork();
}

int _sys_exec() {
    return execve(0);
}

void _sys_cputs(const char *s, uint32 len) {
    printf("%s", s);
}

void _sys_send_sig(int signo) {
    sig_set_pending(current_task, signo);
}

void _sys_reg_sig_hand(int signo, sig_handler_t handler) {
    sig_register_handler(current_task, signo, handler);
}

void _sys_sig_ret() {
    printf("SIG RET!!!\n");
    sig_ret(current_task);
}

void * _sys_sbrk(int sz) {
    //Implement
    return (proc_sbrk(sz));
}

static void *syscalls[9] = {
    &_sys_write,
    &_sys_exit,
    &_sys_fork,
    &_sys_cputs,
    &_sys_exec,
    &_sys_send_sig,
    &_sys_reg_sig_hand,
    &_sys_sig_ret,
    &_sys_sbrk
};

uint32 num_syscalls = 9;

void syscall_handlers(registers_t *regs) {
    if (regs->eax > num_syscalls) {
        return;
    }
    handler_t cb = syscalls[regs->eax - 1];
    current_task->regs2 = regs;
    memcpy(&current_task->regs, regs, sizeof(registers_t));
    // printf("In syscall handlers\n");
    // printf("eax: %x ebx: %x ecx: %x edx: %x \n"
    //         "esi: %x edi: %x eip: %x esp: %x \n"
    //         "eflags: %x int_no: %d ss: %x err_code: %x \n", regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi,
    //     regs->eip, regs->esp, regs->eflags, regs->int_no, regs->ss, regs->err_code);
    uint32 ret = cb(regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
    regs->eax = ret;
}

void init_syscalls() {
    irq_install_handler(SYSCALL_IRQ, &syscall_handlers);
}

