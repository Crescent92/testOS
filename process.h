#ifndef _PROC_H
#define _PROC_H
#include "type.h"
#include "mem.h"

typedef struct process {
    uint32 pid;             /* Process ID */
    uint32 esp;             /* current stack top */
    uint32 ss;              /* Stack segment */
    uint32 kern_stack_base; /* Base address of kernel stack*/
    uint32 kern_stack_top;
    uint32 cr3;             /* Page directory */
    uint32 total_time;      /* Time slice remaining */
    uint32 state;           /* Process state */
    uint32 cpu_time;        /* Cpu time used */
    uint32 sleep_expiry;    /* Sleep expiry */
    char   name[32];        /* Process name */
    struct process *next;   /* Next process in the list */
} process_t;

enum process_state {
    PROC_RUNNING,
    PROC_READY,
    PROC_WAITING,
    PROC_BLOCKED,
    PROC_WAITING_LOCK,
    PROC_SLEEPING
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

process_t *current_task;                /* ptr to current task that is running on CPU. */

#endif
