#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "port.h"
#include <stdint.h>

volatile extern TCB_t *pxCurrentTCB;
volatile uint32_t tick_counter;

TCB_t *task_init(void);
TCB_t **get_top_priority();
void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void), int prio);
void task_switch_context(void);
void task_start_schedulers(void);
void task_delay(uint32_t ticks);

void insert_to_list(TCB_t **list, TCB_t *tcb, int prio);
void insert_to_ready(TCB_t *ready);
void remove_from_list(TCB_t **list, TCB_t *tcb);

extern volatile uint32_t tick_counter;
void unblock_if_finished(void);

#endif