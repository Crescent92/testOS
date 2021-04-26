#ifndef _PROC_H
#define _PROC_H
#include <type.h>
#include <mem/mem.h>
#include <fs/fs.h>
#include <mem/paging.h>
#include <cpu/isr.h>
#include "sig_com.h"


typedef struct process {
    uint32 pid;             /* Process ID */
    uint32 parent_pid;      /* Parent PID */
    uint32 esp;             /* current stack top */
    uint32 ss;              /* Stack segment */
    uint32 kern_stack_base; /* Base address of kernel stack*/
    uint32 kern_stack_top;
    uint32 useresp;         /* User esp */
    uint32 pd_phy_addr;     /* page dir phy addr */
    page_directory_t *cr3;            /* Page directory */
    uint32 total_time;      /* Time slice remaining */
    uint32 state;           /* Process state */
    uint32 cpu_time;        /* Cpu time used */
    uint32 sleep_expiry;    /* Sleep expiry */
    uint32 entry;           /* Proc entry point */
    registers_t regs;       /* Register state */
    registers_t *regs2;     /* pointer to original reg set stored on kernel stack. */
    uint32  usr_heap;
    int sig_pend[SIG_CNT + 1];  /* Pending signals. */
    sig_handler_t   sig_handlers[SIG_CNT + 1];  /* Signal handlers. */
    registers_t sig_return_st;  
    int in_sig;             /* Set if currently handling signal. */
    char   name[32];        /* Process name */
    struct process *next;   /* Next process in the list */
} process_t;

enum process_state {
    PROC_RUNNING,
    PROC_READY,
    PROC_WAITING,
    PROC_BLOCKED,
    PROC_WAITING_LOCK,
    PROC_SLEEPING,
    PROC_PURGE
};

void create_kern_task(void (*fun_ptr)(void), char *name);

void initialize_multitasking();

process_t *get_proc_t_by_pid(uint32 pid);

process_t *get_proc_t_by_name (char *name);

void lock_scheduler();

void unlock_scheduler();

void block_task(int reason);

void unblock_task(process_t *proc);

process_t *get_proc_t_by_name_from_wait_list(char *name);

void schedule();

extern void task_switch(process_t *next_task);

process_t *pop_task_from_ready_list(process_t *task);

void user_thread_entry ();

process_t *current_task;                /* ptr to current task that is running on CPU. */

void start_new_proc (fs_node_t *node);

uint32 fork();

uint32 execve(const char *path);

void *proc_sbrk(uint32 sz);

#endif
