#ifndef SYSCALL_H
#define SYSCALL_H

#include "common.h"

void init_syscalls();

void sys_print_str (char *buf, int sz);

void sys_exit(int32 exit_code);

#endif