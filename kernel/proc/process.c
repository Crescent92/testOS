#include "process.h"
#include <common.h>
#include <fs/fs.h>
#include "elf32.h"
#include <mem/kheap.h>
#include <cpu/gdt.h>

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
extern void switch_usr_mode(uint32 esp);
extern void user_mode_ret(registers_t *);

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
    // sprintf("Hello, I switch! to: %s\n", next_task->name);
    // printf("eax: %x ebx: %x ecx: %x edx: %x \n"
    //         "esi: %x edi: %x eip: %x esp: %x \n"
    //         "eflags: %x int_no: %d ss: %x err_code: %x \n", next_task->regs.eax, next_task->regs.ebx, next_task->regs.ecx, next_task->regs.edx, next_task->regs.esi, next_task->regs.edi,
    //     next_task->regs.eip, next_task->regs.esp, next_task->regs.eflags, next_task->regs.int_no, next_task->regs.ss, next_task->regs.err_code);
    if (next_task->pid != 0) { 
        set_kern_stack(next_task->kern_stack_top);
    }
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

    // process_t *proc = proc_head;
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
    process_t *next_task = find_next_task_to_run();
    // printf("next task to run: %s pid: %d\n", next_task->name, next_task->pid);
    if (next_task->pid > 32) {
        printf("Faulty return from find_task\n");
    }
    if (next_task == current_task) {
        return;
    }
    if (next_task) {
        _schedule(next_task);
    } else {
        _schedule(idle_task);
    }


}

// void schedule() {
//     lock_scheduler();
//     if (postpone_task_sw_counter != 0) {
//         task_sw_postpone_f = 1;
//         goto out;
//         return;
//     }

//     if (current_task->state == PROC_RUNNING) {
//         current_task->state = PROC_READY;
//     }

//     if (current_task->state == PROC_BLOCKED) {
//         put_task_in_wait_list(pop_task_from_ready_list(current_task));
//     }

//     process_t *next_task = find_next_task_to_run();

//     if (current_task == next_task) {
//         goto out;
//         return;
//     }

//     if (next_task->state == PROC_READY) {
//         next_task->state = PROC_RUNNING;
//     }
//     _last_task_sw_time = get_current_time_in_ms_since_boot();
//     _schedule(next_task);
// out:
//     unlock_scheduler();
// }

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
    new_task->cr3 = kernel_directory;
    new_task->pd_phy_addr = kernel_directory->physicalAddr;
    current_task = new_task;

    idle_task = new_task;

   /* last_task_created = new_task;
    proc_head = new_task;*/
}

extern void start_user_mode_thread(uint32 eip, uint32 esp);
void user_thread_entry3(process_t *task) {
    uint32 *uesp = 0;
    uesp = task->useresp;
    --uesp;
    *uesp = (uint32)1;  //argv
    --uesp;
    *uesp = (uint32)1;          //argc
    task->useresp = uesp;

    set_kern_stack(task->kern_stack_top);
    start_user_mode_thread(task->entry, task->useresp);
    // start_user_mode_thread(task->entry, task->esp);
}

void create_kern_task2(char *name, process_t *task) {

    //initialize stack as if returning from a process,i.e, make return address point to new task.
    // asm volatile("cli");
    uint32 usr_addr = 0;
    memcpy(task->name, name, strlen(name));
    
    // vmm_switch_dir(new_task->cr3);
    
    uint32 *kern_stk_ptr = 0;
    task->kern_stack_base = kmalloc_a(4096);


    task->kern_stack_top = (uint32)(((char *)(task->kern_stack_base)) + (4096 - sizeof(uint32)));
    kern_stk_ptr = (uint32 *)task->kern_stack_top;
    
    task->cr3 = vmm_clone_dir5(1);
    task->pd_phy_addr = task->cr3->physicalAddr;
    vmm_switch_dir(task->cr3);

    task->usr_heap = 0;
    //map user heap
    task->usr_heap = USER_HEAP_LOCATION + (2 * VMM_PAGE_SIZE);
    for (usr_addr = USER_HEAP_LOCATION; usr_addr <= task->usr_heap; usr_addr += VMM_PAGE_SIZE) {
        alloc_frame(get_page(usr_addr, 1, current_kernel_directory), 0, 1, 1);
    }
    //map user stack
    for (usr_addr = USER_STACK_LOCATION - 10*VMM_PAGE_SIZE; usr_addr <= USER_STACK_LOCATION; usr_addr += 0x1000) {
        alloc_frame(get_page(usr_addr, 1, current_kernel_directory), 0, 1, 1);
    }
    task->useresp = USER_STACK_LOCATION - sizeof(uint32);
    //eip should be address of the task.
    *kern_stk_ptr = (uint32)task;
    kern_stk_ptr--;

    *kern_stk_ptr = 0xfffffffe;         //return address goes here; eventually use this to exit out of user space process
    kern_stk_ptr--;

    *kern_stk_ptr = (uint32)user_thread_entry3;
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
    task->esp = (uint32)kern_stk_ptr;

    task->state = PROC_READY;

    task->next = NULL;

    if (proc_head == NULL) {
        proc_head = task;
    }
    if (last_task_created != NULL) {
        last_task_created->next = task;
    }
    last_task_created = task;

    proc_tail = task;



    // asm volatile("sti");
    
}

void create_kern_task(void (*fun_ptr)(void), char *name) {
    //create proc struct
    //allocate kern stack
    //save ebx, esi, edi, ebp
    //initialize stack as if returning from a process,i.e, make return address point to new task.
    // asm volatile("cli");
    process_t *new_task = (process_t *)kmalloc(sizeof(process_t));
    memset(new_task, 0, sizeof(process_t));
    new_task->pid = global_pid_count;
    global_pid_count++;
    memcpy(new_task->name, name, strlen(name));
    
    // vmm_switch_dir(new_task->cr3);
    
    uint32 *kern_stk_ptr = 0;
    new_task->kern_stack_base = kmalloc_a(4096);


    new_task->kern_stack_top = (uint32)(((char *)(new_task->kern_stack_base)) + (4096 - sizeof(uint32)));
    kern_stk_ptr = new_task->kern_stack_top;
    new_task->cr3 = vmm_clone_dir5(0);
    new_task->pd_phy_addr = new_task->cr3->physicalAddr;
    // vmm_switch_dir(new_task->cr3);
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



    // asm volatile("sti");
    
}

void user_thread_entry (uint32 esp) {
    switch_usr_mode(esp);
}

static process_t * _new_elf_proc (uint8 *file_d/*, uint32 len*/) {
    printf("spawn new elf proc\n");
    // elf32_hdr_t *hdr = (elf32_hdr_t *)file_d;
    process_t *proc = (process_t *)kmalloc(sizeof(process_t));
    memset(proc, 0, sizeof(process_t));
    proc->pid = global_pid_count;
    global_pid_count++;
    // proc->cr3 = vmm_clone_dir2();
    // vmm_switch_dir(proc->cr3);
    elf32_load(proc, file_d);

    return proc;
}

void start_new_proc (fs_node_t *node) {
    // asm volatile("cli");
    uint8 * file_d = (uint8 *)kmalloc(node->length);
    memset(file_d, 0, node->length);
    if (read_fs(node, 0, node->length, file_d) == node->length) {
        process_t *proc = _new_elf_proc(file_d/*, node->length*/);
        create_kern_task2("test", proc);
    }
    printf("About to exit start new proc...\n");
    //  asm volatile("sti");
}

void user_thread_entry2(process_t *task) {

    set_kern_stack(task->kern_stack_top);

    user_mode_ret(&task->regs);
}

extern void copy_page (uint32 addr, page_directory_t *newdir);

uint32 fork() {
    uint32 *kern_stk_ptr = 0;
    //Disable interrupts
    // asm volatile("cli");
    page_directory_t * temp_dir = current_kernel_directory;
    
    process_t *parent_task = current_task;

    process_t *new_task = (process_t *)kmalloc(sizeof(process_t));
    memset(new_task, 0, sizeof(process_t));
    // memcpy(new_task, parent_task, sizeof(parent_task));
    new_task->pid = global_pid_count;
    global_pid_count++;
    new_task->parent_pid = parent_task->pid;
    strcpy(new_task->name, parent_task->name);
    strcat(new_task->name, ".1");
    new_task->state = PROC_READY;
    new_task->cr3 = vmm_clone_dir5(1);

    
    new_task->pd_phy_addr = new_task->cr3->physicalAddr;
    new_task->useresp = parent_task->useresp;
    memcpy(&new_task->regs, &current_task->regs, sizeof(registers_t));
    new_task->regs.eax = 0;
    new_task->kern_stack_base = kmalloc_a(4096);
    new_task->kern_stack_top = (uint32)(((char *)(new_task->kern_stack_base)) + (4096 - sizeof(uint32)));
    kern_stk_ptr = (uint32 *)new_task->kern_stack_top;
    //arg to user thread entry
    *kern_stk_ptr = (uint32)new_task;
     kern_stk_ptr--;

    *kern_stk_ptr = 0xfffffffe;         //return address goes here; eventually use this to exit out of user space process
    kern_stk_ptr--;

    //eip should be address of the task.
    *kern_stk_ptr = (uint32)(user_thread_entry2);
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


    printf("Setup done. About to exit fork...\n");
    if (current_task == parent_task) {
        
        return new_task->pid;
    } else {
        return 0;
    }
}
extern fs_node_t *find_file_node (char *name);
uint32 execve(const char *path) {
    char *usr_addr = 0;
    fs_node_t *node = find_file_node("test2");
    uint8 * file_d = (uint8 *)kmalloc(node->length);
    memset(file_d, 0, node->length);

    /* zero current user pages. */
    for (usr_addr = USER_STACK_LOCATION - 10*VMM_PAGE_SIZE; usr_addr <= USER_STACK_LOCATION; usr_addr += 0x1000) { 
        memset(usr_addr, 0, VMM_PAGE_SIZE);
    }

    // Zero heap pages
    // for (usr_addr = USER_HEAP_LOCATION; usr_addr <= current_task->usr_heap; usr_addr += VMM_PAGE_SIZE) {
    //     memset(usr_addr, 0, VMM_PAGE_SIZE);
    // }

    for (usr_addr = 0xC00000; usr_addr < 0xC03000; usr_addr += 0x1000) {
        memset(usr_addr, 0, VMM_PAGE_SIZE);
    }

    if (read_fs(node, 0, node->length, file_d) == node->length) {
         elf32_load(current_task, file_d);
    }

    current_task->useresp = USER_STACK_LOCATION - sizeof(uint32);

    // printf("Exec: %s\n", path);

    set_kern_stack(current_task->kern_stack_top);
    uint32 *uesp = 0;
    uesp = current_task->useresp;
    --uesp;
    *uesp = (uint32)1;  //argv
    --uesp;
    *uesp = (uint32)1;          //argc
    current_task->useresp = uesp;

    start_user_mode_thread(current_task->entry, current_task->useresp);
}

extern phy_mem_layout_t phy_mem_;

void proc_exit() {
    char *usr_addr = 0;
    process_t *task_del = current_task;
    current_task->state = PROC_PURGE;
    //free user address space
    for (usr_addr = 0xC00000; usr_addr < 0xF0000000; usr_addr += 0x1000) {
        // printf("free usr_addr: %x\n", usr_addr);
        free_frame(get_page(usr_addr, 0, current_kernel_directory));
    }

    //Delete current task
    put_task_in_wait_list(pop_task_from_ready_list(current_task));
    //Run next available task
    schedule();
}

//sz should already be a multiple of 4k at this point
void *proc_sbrk(uint32 sz) {
    uint32 i;
    uint32 map_cnt = sz / 4096;
    uint32 old_brk = current_task->usr_heap;
    char *addr = (char *)current_task->usr_heap;
    if (addr == 0) {
        old_brk = USER_HEAP_LOCATION;
        addr = USER_HEAP_LOCATION;
    }


    for (i = 0; i < map_cnt; i++) {
        // vmm_map_page(current_kernel_directory, addr, 0, 1);

        
        alloc_frame(get_page(addr, 1, current_kernel_directory), 0, 1, 1);
        
        // vmm_flush_tlb_entry2(addr);
        addr += VMM_PAGE_SIZE;
    }
    current_task->usr_heap = addr;
    return old_brk;
}