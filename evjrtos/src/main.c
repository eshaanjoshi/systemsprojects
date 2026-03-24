#include <avr/io.h>
#include "uart.h"
#include "port.h"
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
        uart_print_str("here1\n");
        task_delay(8);
        PORTB &= ~(1<<PB0);
        uart_print_str("here2\n");
        task_delay(8);
    }
}

void task1(void) {
    while (1) {
        PORTB |= (1<<PB1);
         uart_print_str("here3\n");
        task_delay(4);
        PORTB &= ~(1<<PB1);
        uart_print_str("here4\n");
        task_delay(4);
    }
}






int main(void) {

    DDRB |= (1 << PB0) | (1 << PB1);
    PORTB &= ~((1 << PB0) | (1 << PB1));
    TCB_t *init_task_tcb = task_init();
    uart_init();

    task_create(&task0_tcb, task0_stack, task0, 1);
    task_create(&task1_tcb, task1_stack, task1, 2);


    prvSetupTimerInterrupt();

    //uart_print_hex(tick_counter);    

    pxCurrentTCB = *get_top_priority();//task_list[0];
    sei();
    portRESTORE_CONTEXT();
    asm volatile("ret");

    while(1);
}