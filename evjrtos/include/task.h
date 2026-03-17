#ifndef TASK_H
#define TASK_H
#define STACK_SIZE 128
#define MAX_TASKS 6
#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>
typedef enum{
	READY,
	RUNNING
} State;


struct TCB_h{
	uint8_t* stack_pointer;
	int priority;
	uint8_t stack[STACK_SIZE];
	State state;
	void (*fptr) (void);
};

typedef struct TCB_h TCB;
typedef TCB* tcbptr;
extern TCB* volatile current_tcb;
extern TCB* volatile next_tcb;


tcbptr init_main_tcb(void);
void rtos_init(void);
extern TCB task_pool[MAX_TASKS];
extern uint8_t task_pool_free[MAX_TASKS];
tcbptr task_create(void (*fptr) (void), int prio);
#endif