#ifndef SYSCALL_H
#define SYSCALL_H

#include <common.h>

typedef void (*sig_handler_t)(void);

void sys_print_str(char *buf, int sz);
void sys_print_str2(char *buf);

int sys_fork();

int sys_exec();

void sys_exit(int32 exit_code);

void sys_cputs(const char *s, uint32 len);

void sys_sig_ret();

void sys_reg_signal_hdlr(int signo, sig_handler_t handler);

void sys_send_sig(int signo);

char *sys_sbrk(uint32 sz);

#define _syscall0(name)                           \
	static inline int32_t syscall_##name()        \
	{                                             \
		int32_t ret;                              \
		__asm__ __volatile__("int $0x80"          \
							 : "=a"(ret)          \
							 : "0"(__NR_##name)); \
		return ret;                               \
	}

#endif