#include <avr/io.h>
#include "uart.h"
#include "port.h"

#define STACK_SIZE 128

static uint8_t task0_stack[STACK_SIZE];
static uint8_t task1_stack[STACK_SIZE];

static TCB_t task0_tcb;
static TCB_t task1_tcb;

void task0(void) {
    while (1) {
        uart_print_str("Task 0\r\n");
        portSAVE_CONTEXT();
        pxCurrentTCB = &task1_tcb;
        portRESTORE_CONTEXT();
        asm volatile("ret");
    }
}

void task1(void) {
    while (1) {
        uart_print_str("Task 1\r\n");
        portSAVE_CONTEXT();
        pxCurrentTCB = &task0_tcb;
        portRESTORE_CONTEXT();
        asm volatile("ret");
    }
}

int main(void) {
    uart_init();

    task_create(&task0_tcb, task0_stack, task0);
    task_create(&task1_tcb, task1_stack, task1);

    pxCurrentTCB = &task0_tcb;
    portRESTORE_CONTEXT();
    asm volatile("ret");

    while(1);
}