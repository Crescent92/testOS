#ifndef _SIGNALS_H
#define _SIGNALS_H

#include "process.h"
#include <cpu/isr.h>
#include "sig_com.h"


#define SIGABRT 0
#define SIGFPE  1
#define SIGILL  2
#define SIGINT  3
#define SIGSEGV 4
#define SIGTERM 5
#define SIGRET  6

void sig_set_pending(process_t *proc, int signo);
void sig_handle_pending();
void sig_register_handler(process_t *proc, int signo, sig_handler_t handler);
void sig_ret(process_t *proc);

#endif