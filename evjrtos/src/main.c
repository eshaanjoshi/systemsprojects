#include <avr/io.h>
#include "uart.h"
#include "port.h"

#include <avr/interrupt.h>


static uint8_t task0_stack[STACK_SIZE];
static uint8_t task1_stack[STACK_SIZE];

static TCB_t task0_tcb;
static TCB_t task1_tcb;
uint8_t restarts = 0;

void task0(void) {
    volatile uint8_t count = 0;
    while (1) {
        count++;
        uart_print_str("A:");
        uart_print_hex(count);
        uart_print_newline();
    }
}

void task1(void) {
    volatile uint8_t count2 = 0;
    while (1) {
        uart_print_str("B:");
        uart_print_hex(count2);
        uart_print_newline();
        count2--;
    }
}





int main(void) {
    task_init();
    uart_init();

    task_create(&task0_tcb, task0_stack, task0);
    task_create(&task1_tcb, task1_stack, task1);

    prvSetupTimerInterrupt();


    pxCurrentTCB = task_list[0];
    sei();
    portRESTORE_CONTEXT();
    asm volatile("ret");

    while(1);
}