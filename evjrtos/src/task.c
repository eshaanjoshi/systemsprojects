#include "task.h"

TCB task_pool[MAX_TASKS];
uint8_t task_pool_free[MAX_TASKS];

TCB* volatile current_tcb;
TCB* volatile next_tcb;

void rtos_init(){
    for(int i = 0; i < MAX_TASKS; i++){
        task_pool_free[i] = 1;
    }
}

tcbptr task_create(void (*fptr) (void), int prio){
    int task_pool_id = -1;
    for(int i = 0; i < MAX_TASKS; i++){
        if (task_pool_free[i]){
            task_pool_free[i] = 0;
            task_pool_id = i;
            break;
        }
    }
    if (task_pool_id == -1) return NULL;

    TCB* task = &task_pool[task_pool_id];
    task->priority = prio;
    task->state = READY;
    task->fptr = fptr;

    // Initialize stack pointer to the end of stack
    uint8_t *sp = &task->stack[STACK_SIZE-1];
    uint16_t addr = (uint16_t)fptr;
    
    // Push PC (return address) - matches order from context_switch
    *(sp--) = addr >> 8;     // PC high
    *(sp--) = addr & 0xff;   // PC low
    
    // Push R0
    *(sp--) = 0x00;
    
    // Push SREG (with interrupts enabled - 0x80)
    *(sp--) = 0x80;
    
    // Push R1
    *(sp--) = 0x00;
    
    // Push R2 through R31 (30 registers)
    for(int i = 2; i <= 31; i++)
        *(sp--) = 0x00;
    
    // Store the stack pointer (pointing to the last pushed value)
    task->stack_pointer = sp + 1;
    
    return task;
}

tcbptr init_main_tcb(void){
    tcbptr task = &task_pool[MAX_TASKS-1];
    task_pool_free[MAX_TASKS-1] = 0;
    task->state = RUNNING;
    task->fptr = NULL;
    
    // Get current stack pointer for main task
    uint8_t* sp;
    asm volatile ("in %0, __SP_L__ \n\t" : "=r"(sp));
    task->stack_pointer = sp;
    
    return task;
}