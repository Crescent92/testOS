#ifndef SEMA_H
#define SEMA_H

#include <proc/process.h>
#include <type.h>
#include <mem/mem.h>

typedef struct {
    int max_count;
    int current_count;
    process_t *first_waiting_task;
    process_t *last_waiting_task;
} semaphore_t;

#endif