#include <signal_tramp.h>
#include <syscalls.h>
#include <printf.h>

void sig_trampoline(void) {
    sys_sig_ret();
}