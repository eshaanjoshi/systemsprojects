#include "port.h"
#include "uart.h"
#include <avr/interrupt.h>
TCB_t *task_list[MAX_TASKS];
uint8_t task_count = 0;
uint8_t free_list[MAX_TASKS];
uint8_t current_index = 0;
TCB_t *pxCurrentTCB;

//void TIMER1_COMPA_vect(void) __attribute__((signal, naked));
void vPortYieldFromTick(void) __attribute__ ((naked));


void task_init(void) {
    for(int i = 0; i < MAX_TASKS; i++){
        free_list[i] = 0;
    }
}

int _get_free_task(void){
    for(int i = 0; i < MAX_TASKS; i++){
        if (free_list[i] == 0){
            return i;
        }
    }
    return -1;
}

uint8_t *pxPortInitialiseStack(uint8_t *pxTopOfStack, void (*pxCode)(void)) {
    uint16_t usAddress = (uint16_t)pxCode;

    // PC - low byte first
    *pxTopOfStack = (uint8_t)(usAddress & 0x00ff);
    pxTopOfStack--;
    usAddress >>= 8;
    *pxTopOfStack = (uint8_t)(usAddress & 0x00ff);
    pxTopOfStack--;

    // Simulate portSAVE_CONTEXT layout
    *pxTopOfStack = 0x00; // r0
    pxTopOfStack--;
    *pxTopOfStack = portFLAGS_INT_ENABLED; // SREG
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r1 - gcc expects zero
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r2
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r3
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r4
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r5
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r6
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r7
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r8
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r9
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r10
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r11
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r12
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r13
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r14
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r15
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r16
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r17
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r18
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r19
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r20
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r21
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r22
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r23
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r24
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r25
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r26
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r27
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r28
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r29
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r30
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r31
    pxTopOfStack--;

    return pxTopOfStack;
}

void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void)) {
    uint8_t *sp = stack + STACK_SIZE - 1;
    uint16_t addr = (uint16_t)task_func;
    int i = _get_free_task();
    if (i<0) return;//TODO make halt
    task_list[i] = tcb;
    free_list[i] = 1;
    task_count++;

    uint8_t *top = stack+STACK_SIZE-1;
    tcb->pxTopOfStack = pxPortInitialiseStack(top, task_func);
}

void prvSetupTimerInterrupt(void){
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A  = 2499; 
    TIMSK1 = (1 << OCIE1A);  
}

void vTaskSwitchContext(void){
     while(1){
        current_index = (current_index+1) % MAX_TASKS;
        
        if (free_list[current_index] != 0) {
            break;
        }
    }
    pxCurrentTCB = task_list[current_index];
}

ISR(TIMER1_COMPA_vect, ISR_NAKED){
    vPortYieldFromTick();
    asm volatile("reti");
}

void vPortYieldFromTick(void){
    portSAVE_CONTEXT();
    //vTaskIncrementTick();
    vTaskSwitchContext();
    portRESTORE_CONTEXT();
    asm volatile("ret");
}