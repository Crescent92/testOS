
#include <printf.h>
#include <syscalls.h>
#include <signal_tramp.h>
#include <malloc.h>

// void sig_hello(void) {
//     cprintf("Henlo!\n");
//     // while (1)
//     // {
//     //     cprintf("Signal wovking!\n");
//     // }
// }

void main(int argc, char **argv) {
    cprintf("Testing Malloc!\n");
    // cprintf("argc: %d\n", argc);
    // // while (1)
    // // {
    //     sys_reg_signal_hdlr(1, sig_hello);
    //     cprintf("Exec working!\n");
    //     sys_send_sig(1);
    //     cprintf("Signal handled!\n");
    // char *ptr = kmem_alloc(10);
    // cprintf("ptr: %x\n", ptr);

    // while(1) {

    // }
}