#include "process.h"
#include "common.h"

#define MILLISECONDS       1000
#define MICROSECONDS       ((MILLISECONDS) * (MILLISECONDS))
#define NANOSECONDS        ((MICROSECONDS) * (MILLISECONDS))
#define TIME_SLICE_LENGTH  50000000 // each task runs for 50ms

uint32 global_pid_count = 0;            /* pid count */
uint32 irq_disable_counter = 0;
uint32 postpone_task_sw_counter = 0;
uint32 task_sw_postpone_f = 0;
uint32 prev_sw_time = 0;
uint32 _last_task_sw_time = 0;
uint32 time_bw_ticks = 1000000;


process_t *last_task_created = NULL;    /* ptr to end of process list */
process_t *idle_task = NULL;
/* For list of processes in blocked state. */
process_t *wait_list_head = NULL;      /* ptr to head of process list */
process_t *wait_list_tail = NULL;      /* ptr to tail of process list. */

/* For list of processes that are ready to run. */
process_t *proc_head = NULL;            /* ptr to head of process list */
process_t *proc_tail = NULL;            /* ptr to tail of process list. */

extern void enable_interrupts();
extern void clear_interrupts();
extern uint32 time_in_ms_since_boot;

uint32 get_current_time_in_ms_since_boot() {
    return time_in_ms_since_boot;
}

void update_cpu_time_used(process_t *proc) {
    uint32 current_time = get_current_time_in_ms_since_boot();
    uint32 elapsed = current_time - _last_task_sw_time;
    proc->cpu_time += elapsed;
}

void lock_scheduler() {
    clear_interrupts();
    irq_disable_counter++;
    postpone_task_sw_counter++;
}

void unlock_scheduler() {
    postpone_task_sw_counter--;
    if (postpone_task_sw_counter == 0) {
        if (task_sw_postpone_f != 0) {
            task_sw_postpone_f = 0;
            schedule();
        }
    }
    irq_disable_counter--;
    if (irq_disable_counter == 0) {
        enable_interrupts();
    }
}

process_t *get_proc_t_by_name_from_wait_list(char *name) {
    process_t *proc = wait_list_head;
    if (proc == NULL) {
        return 0;
    }

    while ((proc != NULL) && strcmp(proc->name, name)) {
        proc = proc->next;
    }

    return proc;
}

process_t *get_proc_t_by_pid_from_wait_list(uint32 pid) {
    process_t *proc = wait_list_head;
    if (proc == NULL) {
        return 0;
    }

    while ((proc != NULL ) && (proc->pid != pid) ) {
        proc = proc->next;
    }

    return proc;
}

process_t *get_proc_t_by_pid(uint32 pid) {
    process_t *proc = proc_head;
    if (proc == NULL) {
        return 0;
    }

    while ((proc != NULL ) && (proc->pid != pid) ) {
        proc = proc->next;
    }

    return proc;
}

process_t *get_proc_t_by_name (char *name) {
    process_t *proc = proc_head;
    if (proc == NULL) {
        return 0;
    }

    while ((proc != NULL) && strcmp(proc->name, name)) {
        proc = proc->next;
    }

    return proc;
}

void schedule_task(process_t *task) {
    if (task == idle_task) {
        return;
    }

    if (task->state != PROC_READY) {
        return;
    }

    if (proc_tail == NULL) {
        proc_head = proc_tail = task;
    } else {
        proc_tail->next = task;
        proc_tail = task;
    }
}

/*
process_t *get_first_task_to_run() {
     process_t *next_task = NULL;

    if (current_task == NULL) {
        goto done;
    }

    if (proc_head == NULL) {
        goto done;
    }

    while (proc_head != NULL) {
        next_task = proc_head;
        proc_head = next_task->next;
        if (proc_head == NULL) {
            proc_tail = NULL;
        }
        if (next_task->state == PROC_READY) {
            goto done;
        }    
    }
done:
    return next_task;
}
*/

process_t *get_first_task_to_run() {
    process_t *next_task = NULL;

    if (current_task == NULL) {
        goto done;
    }

    if (proc_head == NULL) {
        goto done;
    }
    next_task = current_task->next;
    if (next_task == NULL) {
        next_task = proc_head;
    }
done:
    return next_task;
}

process_t *_find_next_task_to_run() {
    process_t *next_task = get_first_task_to_run();

    if (next_task) {
        return next_task;
    } else if (current_task && (current_task->state == PROC_READY)) {
        return current_task;
    } else {
        return idle_task;
    }
}

process_t *find_next_task_to_run() {
    return _find_next_task_to_run();
}

/*
process_t *find_next_task_to_run() {
    process_t *next_task = NULL;

    if (current_task == NULL) {
        goto done;
    }

    if (proc_head == NULL) {
        goto done;
    }

    while (proc_head != NULL) {
        next_task = proc_head;
        proc_head = next_task->next;
        if (proc_head == NULL) {
            proc_tail = NULL;
        }
        if (next_task->state == PROC_READY) {
            goto done;
        }
        
    }
done:
    return next_task;
}
*/

void _schedule(process_t *next_task) {
    task_switch(next_task);
}

void put_task_in_wait_list(process_t *task) {
    if (wait_list_head == NULL) {
        wait_list_head = wait_list_tail = task;
    } else {
        wait_list_tail->next = task;
        wait_list_tail = task;
    }
}

void put_task_in_ready_list(process_t *task) {
    task->next = NULL;
    if (proc_head == NULL) {
        proc_head = proc_tail = task;
    } else {
        proc_tail->next = task;
        proc_tail = task;
    }
}

process_t *pop_task_from_wait_list(process_t *task) {
    //find task in wait list
    //pop it

    process_t *current_t = NULL,
              *prev_task = NULL;
    current_t = prev_task = wait_list_head;
    if ((current_t != NULL) && (current_t == task)) {
        current_t->next = NULL;
        wait_list_head = wait_list_head->next;
        goto done;
    }
    while ((current_t != NULL) && (current_t != task)) {
        prev_task = current_t;
        current_t = current_t->next;
    }

    if (current_t == NULL) {
        goto done;
    }

    prev_task->next = current_t->next;
    current_t->next = NULL;
done:
    return current_t;

}

process_t *pop_task_from_ready_list(process_t *task) {
    //find task in wait list
    //pop it

    process_t *proc = proc_head;
    process_t *current_t = NULL,
              *prev_task = NULL;
    current_t = prev_task = proc_head;
    if ((current_t != NULL) && (current_t == task)) {
        proc_head = proc_head->next;
        current_t->next = NULL;
        goto done;
    }
    while ((current_t != NULL) && (current_t != task)) {
        prev_task = current_t;
        current_t = current_t->next;
    }

    if (current_t == NULL) {
        goto done;
    }

    prev_task->next = current_t->next;
    current_t->next = NULL;
done:
    return current_t;

}

void block_task(int reason) {
    lock_scheduler();
    current_task->state = reason;
    schedule();
    unlock_scheduler();
}

void unblock_task(process_t *proc) {
    lock_scheduler();
    proc->state = PROC_READY;
    //put task in ready to run list
    schedule_task(pop_task_from_wait_list(proc));
    unlock_scheduler();
}

void _nano_sleep_until(uint32 until) {
    lock_scheduler();
    if (until < get_current_time_in_ms_since_boot()) {
        unlock_scheduler();
        return;
    }

    current_task->sleep_expiry = until;

    put_task_in_wait_list(pop_task_from_ready_list(current_task));

    unlock_scheduler();

    block_task(PROC_SLEEPING);
}

void nano_sleep (uint32 nanosec) {
    _nano_sleep_until(get_current_time_in_ms_since_boot() + nanosec);
}

void sleep (uint32 sec) {
    _nano_sleep_until(get_current_time_in_ms_since_boot() * 1000000000);
}

void timer_proc_wait_task_cb() {
    process_t *next_task,
              *this_task;
    lock_scheduler();
    time_in_ms_since_boot += time_bw_ticks;

    next_task = wait_list_head;
    wait_list_head = NULL;

    while (next_task != NULL) {
        this_task = next_task;
        next_task = this_task->next;

        if (this_task->state == PROC_SLEEPING) {
            if ((this_task->sleep_expiry > 0) && (this_task->sleep_expiry <= get_current_time_in_ms_since_boot()) ) {
                this_task->sleep_expiry = 0;
                unblock_task(this_task);
            } else {
                this_task->next = wait_list_head;
                wait_list_head = this_task;
            }
        }
    }
    unlock_scheduler();
}

void schedule() {
    if (postpone_task_sw_counter != 0) {
        task_sw_postpone_f = 1;
        return;
    }

    if (current_task->state == PROC_RUNNING) {
        current_task->state = PROC_READY;
    }

    if (current_task->state == PROC_BLOCKED) {
        put_task_in_wait_list(pop_task_from_ready_list(current_task));
    }

    process_t *next_task = find_next_task_to_run();

    if (current_task == next_task) {
        return;
    }

    if (next_task->state == PROC_READY) {
        next_task->state = PROC_RUNNING;
    }
    _last_task_sw_time = get_current_time_in_ms_since_boot();
    _schedule(next_task);
}

/*
void schedule() {
    //find a task to run from "ready to run" list
    //set state to running
    //change previous task's state to ready
    if (current_task->state == PROC_RUNNING) {
        current_task->state = PROC_READY;
    }

    if (current_task->state == PROC_WAITING) {
        //remove from ready list and put in blocked list

    }

    process_t *next_task = find_next_task_to_run();

    if (next_task == NULL) {
        if (current_task && (current_task->state == PROC_READY)) {
            next_task = current_task;
        } 
    }

    if (current_task != next_task) {
        schedule_task(current_task);
    }

    if (next_task == NULL) {
        next_task = idle_task;
    }

    if (next_task->state == PROC_READY) {
        next_task->state = PROC_RUNNING;
    }
    
    _schedule(next_task);
}
*/

void initialize_multitasking() {
    process_t *new_task = (process_t *)kmalloc(sizeof(process_t));
    memset(new_task, 0,sizeof(process_t));
    memcpy(new_task->name, "idle", 5);
    new_task->pid = global_pid_count;
    global_pid_count++;

    new_task->next = NULL;

    new_task->state = PROC_RUNNING;

    current_task = new_task;

    idle_task = new_task;

   /* last_task_created = new_task;
    proc_head = new_task;*/
}

void create_kern_task(void (*fun_ptr)(void), char *name) {
    //create proc struct
    //allocate kern stack
    //save ebx, esi, edi, ebp
    //initialize stack as if returning from a process,i.e, make return address point to new task.

    process_t *new_task = (process_t *)kmalloc(sizeof(process_t));
    memset(new_task, 0, sizeof(process_t));
    new_task->pid = global_pid_count;
    global_pid_count++;

    memcpy(new_task->name, name, strlen(name));

    uint32 *kern_stk_ptr = 0;
    new_task->kern_stack_base = kmalloc(4096);


    new_task->kern_stack_top = (uint32)(((char *)(new_task->kern_stack_base)) + (4096 - sizeof(uint32)));
    kern_stk_ptr = new_task->kern_stack_top;

    //eip should be address of the task.
    *kern_stk_ptr = (uint32)fun_ptr;
    kern_stk_ptr--;

    //ebx
    *kern_stk_ptr = 0;
    kern_stk_ptr--;

    //esi
    *kern_stk_ptr = 0;
    kern_stk_ptr--;

    //edi
    *kern_stk_ptr = 0;
    kern_stk_ptr--;

    //ebp
    *kern_stk_ptr = 0;

    //set current stack pointer
    new_task->esp = (uint32)kern_stk_ptr;

    new_task->state = PROC_READY;

    new_task->next = NULL;

    if (proc_head == NULL) {
        proc_head = new_task;
    }
    if (last_task_created != NULL) {
        last_task_created->next = new_task;
    }
    last_task_created = new_task;

    proc_tail = new_task;
}
