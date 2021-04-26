#include "sema.h"

void put_task_in_sem_wait_list(process_t *task, semaphore_t *semaphore) {
    if (semaphore->first_waiting_task == NULL) {
        semaphore->first_waiting_task = task;
    } else {
        semaphore->last_waiting_task->next = task;
    }
    semaphore->last_waiting_task = task;
}

semaphore_t *create_semaphore(uint32 max_count) {
    semaphore_t *semaphore = NULL;

    semaphore = (semaphore_t *)kmalloc(sizeof(semaphore));

    if (semaphore != NULL) {
        semaphore->max_count = max_count;
        semaphore->current_count = 0;
        semaphore->first_waiting_task = NULL;
        semaphore->last_waiting_task = NULL;
    }

    return semaphore;
}

semaphore_t *create_mutex() {
    return create_semaphore(1);
}

void acquire_semaphore(semaphore_t *semaphore) {
    lock_scheduler();
    if (semaphore->current_count < semaphore->max_count) {
        semaphore->current_count++;
    } else {
        //put in waiting list
        put_task_in_sem_wait_list(pop_task_from_ready_list(current_task), semaphore);
        block_task(PROC_WAITING_LOCK);
    }
    unlock_scheduler();
}

void acquire_mutex(semaphore_t *semaphore) {
    acquire_semaphore(semaphore);
}

void release_semaphore(semaphore_t *semaphore) {
    lock_scheduler();

    if (semaphore->first_waiting_task != NULL) {
        process_t *task = semaphore->first_waiting_task;
        semaphore->first_waiting_task = task->next;
        task->next = NULL;
        unblock_task(task);
    } else {
        semaphore->current_count--;
    }

    unlock_scheduler();
}

void release_mutex(semaphore_t *semaphore) {
    release_semaphore(semaphore);
}