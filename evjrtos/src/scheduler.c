#include "scheduler.h"
#include "port.h"
#include "uart.h"


TCB_t *task_list[MAX_TASKS];
uint8_t task_count = 0;
uint8_t free_list[MAX_TASKS];
uint8_t current_index = 0;
TCB_t *pxCurrentTCB;

volatile uint32_t tick_counter=0;


void task_delay(uint32_t ticks){
    pxCurrentTCB->state = BLOCKED;
    pxCurrentTCB->wake_time = tick_counter+ticks;
    //vPortYieldFromTick();
}
void idle_task(void){
    while(1);
}

static uint8_t idle_stack[STACK_SIZE];
static TCB_t idle_tcb;

void task_init(void) {
    for(int i = 0; i < MAX_TASKS; i++){
        free_list[i] = 0;
    }
    task_create(&idle_tcb, idle_stack, idle_task);
}

int _get_free_task(void){
    for(int i = 0; i < MAX_TASKS; i++){
        if (free_list[i] == 0){
            return i;
        }
    }
    return -1;
}

void unblock_if_finished(void){
    for(int i = 0; i < MAX_TASKS; i++){
        if (free_list[i] != 0 &&
            task_list[i]->state == BLOCKED &&
            task_list[i]->wake_time <= tick_counter){
            task_list[i]->state = READY;
            task_list[i]->wake_time = 0;
        }
    }
}

void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void)) {
    int i = _get_free_task();
    if (i<0) return;//TODO make halt
    task_list[i] = tcb;
    free_list[i] = 1;
    task_count++;

    uint8_t *top = stack+STACK_SIZE-1;
    tcb->state = READY;
    tcb->wake_time = 0;
    tcb->pxTopOfStack = pxPortInitialiseStack(top, task_func);
}


void task_switch_context(void){
    uint8_t checked = 0;
     while(1){
        current_index = (current_index+1) % MAX_TASKS;
        
        if (free_list[current_index] != 0 && task_list[current_index]->state==READY) {
            break;
        }
        checked++;
        if(checked >= MAX_TASKS){
            return;
        }
    }
    pxCurrentTCB = task_list[current_index];
}