#include "signals.h"



void sig_set_pending(process_t *proc, int signo) {
    proc->sig_pend[signo - 1] = 1;
}

void sig_handle_pending() {
    if(current_task->in_sig) {
        return;
    }
    uint32 *uesp = 0;
    uint32 i = 0;
    //Find first pending signal handler and execute.
    while (i < SIG_CNT) {
        if (current_task->sig_pend[i] && current_task->sig_handlers) {
            current_task->sig_pend[i] = 0;
            current_task->sig_return_st = current_task->regs;
            current_task->in_sig = 1;
            // current_task->regs.eip = (uint32)current_task->sig_handlers[i];
            current_task->regs2->eip = (uint32)current_task->sig_handlers[i];

            uesp = current_task->regs2->esp; //get useresp
            // --uesp;
            // *uesp = i;
            --uesp;
            *uesp = (uint32)current_task->sig_handlers[SIGRET - 1]; //store return addr of user space trampoline. Registered on proc entry/crt0.c
            // uesp--;
            current_task->regs2->esp = uesp;
            break;
        }
        i++;
    }

   
}

void sig_register_handler(process_t *proc, int signo, sig_handler_t handler) {
    // printf("Hello! I register sig handler\n");
    if (proc->sig_handlers[signo - 1]) {
        return;
    }
    proc->sig_handlers[signo - 1] = handler;
} 


void sig_ret(process_t *proc) {
    proc->in_sig = 0;

    // proc->regs = proc->sig_return_st;
    memcpy(proc->regs2, &proc->sig_return_st, sizeof(proc->regs));
}