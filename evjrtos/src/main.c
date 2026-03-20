#include <avr/io.h>
#include "uart.h"
#include "port.h"
#include "scheduler.h"
#include "scheduler.h"
#include <avr/interrupt.h>
#include <util/delay.h>

static uint8_t task0_stack[STACK_SIZE];
static uint8_t task1_stack[STACK_SIZE];

static TCB_t task0_tcb;
static TCB_t task1_tcb;
uint8_t restarts = 0;

void task0(void) {
    while (1) {
        PORTB |= (1<<PB0);
        task_delay(8);
        PORTB &= ~(1<<PB0);
        task_delay(8);
    }
}

void task1(void) {
    while (1) {
        PORTB |= (1<<PB1);
        task_delay(8);
        PORTB &= ~(1<<PB1);
        task_delay(8);
    }
}






int main(void) {

    DDRB |= (1 << PB0) | (1 << PB1);
    PORTB &= ~((1 << PB0) | (1 << PB1));
    task_init();
    uart_init();

    task_create(&task0_tcb, task0_stack, task0);
    task_create(&task1_tcb, task1_stack, task1);


    prvSetupTimerInterrupt();

    //uart_print_hex(tick_counter);    

    pxCurrentTCB = task_list[0];
    sei();
    portRESTORE_CONTEXT();
    asm volatile("ret");

    while(1);
}