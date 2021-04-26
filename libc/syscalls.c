#include <syscalls.h>


extern uint32 asm_syscall (uint32 id, uint32 p1, uint32 p2, uint32 p3, uint32 p4, uint32 p5);

#define SYSCALL0(id)                        asm_syscall (id, 0, 0, 0, 0, 0);
#define SYSCALL1(id, p1)                    asm_syscall (id, p1, 0, 0, 0, 0);
#define SYSCALL2(id, p1, p2)                asm_syscall (id, p1, p2, 0, 0, 0);
#define SYSCALL3(id, p1, p2, p3)            asm_syscall (id, p1, p2, p3, 0, 0);
#define SYSCALL4(id, p1, p2, p3, p4)        asm_syscall (id, p1, p2, p3, p4, 0);
#define SYSCALL5(id, p1, p2, p3, p4, p5)    asm_syscall (id, p1, p2, p3, p4, p5);

void sys_print_str(char *buf, int sz) {
    SYSCALL2(1, (uint32)buf, sz);
}

void sys_print_str2(char *buf) {
    SYSCALL1(1, (uint32)buf);
}
void sys_exit(int32 exit_code) {
    SYSCALL1(2, exit_code);
}

int sys_fork() {
    return SYSCALL0(3);
}

int sys_exec() {
    return SYSCALL0(5);
}

void sys_cputs(const char *s, uint32 len) {
    SYSCALL2(4, (uint32)s, len);
}

void sys_send_sig(int signo) {
    SYSCALL1(6, signo);
}

void sys_reg_signal_hdlr(int signo, sig_handler_t handler) {
    SYSCALL2(7, signo, handler);
}

void sys_sig_ret() {
    SYSCALL0(8);
}

char *sys_sbrk(uint32 sz) {
    char *ptr = (char *)SYSCALL1(9, sz);
    return (ptr);
}