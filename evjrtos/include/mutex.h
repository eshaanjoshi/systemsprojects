#ifndef MUTEX_H
#define MUTEX_H
#define NEVER 0xFFFFFF
#include <stdint.h>
#include "port.h"
#include "scheduler.h"

typedef struct Mutex_t {
    TCB_t *owner;
    TCB_t *blocked_list;
    struct Mutex_t *next;
} Mutex_t;

int is_free(Mutex_t *m);
int mutex_lock(Mutex_t *m, uint32_t timeout);
int mutex_unlock(Mutex_t *m);
void mutex_check_timeouts();
void mutex_init(Mutex_t *m);
#endif