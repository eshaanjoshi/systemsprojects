// #include <avr/io.h>
// #include "uart.h"
// #include "port.h"
// #include "scheduler.h"
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include "mutex.h"


// static Mutex_t test_mutex;
// static uint8_t task0_stack[STACK_SIZE];
// static uint8_t task1_stack[STACK_SIZE];

// static TCB_t task0_tcb;
// static TCB_t task1_tcb;
// uint8_t restarts = 0;

// void task0(void) {
//     while (1) {
//         mutex_lock(&test_mutex, NEVER);
//         PORTB |= (1<<PB0);
//         uart_print_str("here1\n");
        
//         task_delay(8);
        
//         PORTB &= ~(1<<PB0);
//         uart_print_str("here2\n");
//         task_delay(8);
//         mutex_unlock(&test_mutex);
//     }
// }

// void task1(void) {
//     while (1) {
//         mutex_lock(&test_mutex, NEVER);
//         PORTB |= (1<<PB1);
//         uart_print_str("here3\n");
        
//         task_delay(4);
//         PORTB &= ~(1<<PB1);
//         uart_print_str("here4\n");
//         task_delay(4);
//         mutex_unlock(&test_mutex);
//     }
// }






// int main(void) {

//     DDRB |= (1 << PB0) | (1 << PB1);
//     PORTB &= ~((1 << PB0) | (1 << PB1));
//     TCB_t *init_task_tcb = task_init();
//     uart_init();
//     mutex_init(&test_mutex);
//     task_create(&task0_tcb, task0_stack, task0, 1);
//     task_create(&task1_tcb, task1_stack, task1, 2);


//     prvSetupTimerInterrupt();

//     //uart_print_hex(tick_counter);    

//     pxCurrentTCB = *get_top_priority();//task_list[0];
//     sei();
//     portRESTORE_CONTEXT();
//     asm volatile("ret");

//     while(1);
// }


#include <avr/io.h>
#include "uart.h"
#include "port.h"
#include "scheduler.h"
#include <avr/interrupt.h>
#include "mutex.h"

static Mutex_t test_mutex;
static uint8_t task_low_stack[STACK_SIZE];
static uint8_t task_high_stack[STACK_SIZE];

static TCB_t task_low_tcb;
static TCB_t task_high_tcb;

// LOW priority task (priority 1) - holds mutex
void task_low(void) {
    while (1) {
        mutex_lock(&test_mutex, NEVER);
        
        // Hold mutex for a long time
        PORTB |= (1<<PB0);
        uart_print_str("LOW: Holds mutex\n");
        
        // Long delay while holding mutex
        for(int i = 0; i < 3; i++) {
            task_delay(20);
            uart_print_str("LOW: Still holding\n");
        }
        
        PORTB &= ~(1<<PB0);
        uart_print_str("LOW: Releases mutex\n");
        mutex_unlock(&test_mutex);
        
        task_delay(10);
    }
}

// HIGH priority task (priority 2) - tries to get mutex
void task_high(void) {
    while (1) {
        uart_print_str("HIGH: Trying to lock\n");
        
        if(mutex_lock(&test_mutex, NEVER) == 1) {
            PORTB |= (1<<PB1);
            uart_print_str("HIGH: Got mutex!\n");
            task_delay(10);
            PORTB &= ~(1<<PB1);
            mutex_unlock(&test_mutex);
            uart_print_str("HIGH: Released\n");
        }
        
        task_delay(5);
    }
}

int main(void) {
    DDRB |= (1 << PB0) | (1 << PB1);
    PORTB &= ~((1 << PB0) | (1 << PB1));
    
    TCB_t *init_task_tcb = task_init();
    uart_init();
    mutex_init(&test_mutex);
    
    task_create(&task_low_tcb, task_low_stack, task_low, 1);   // Lower priority
    task_create(&task_high_tcb, task_high_stack, task_high, 2); // Higher priority
    
    prvSetupTimerInterrupt();
    
    pxCurrentTCB = *get_top_priority();
    sei();
    portRESTORE_CONTEXT();
    asm volatile("ret");
    
    while(1);
}