#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "port.h"
#include <stdint.h>

volatile extern TCB_t *pxCurrentTCB;
volatile extern TCB_t *task_list[MAX_TASKS];
extern uint8_t task_count;
volatile extern uint8_t free_list[MAX_TASKS];
volatile extern uint8_t current_index;


void task_init(void);
void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void));
void task_switch_context(void);
void task_start_schedulers(void);
void task_delay(uint32_t ticks);

extern volatile uint32_t tick_counter;
void unblock_if_finished(void);

#endif