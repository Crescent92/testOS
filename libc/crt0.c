
#include <common.h>
#include "syscalls.h"
#include <signal_tramp.h>
extern int main(int, char**);

void __entry(int argc, char **argv) {
    //register sys_ret()
    sys_reg_signal_hdlr(SIGRET, sig_trampoline);
    exit(main(argc, argv));
}