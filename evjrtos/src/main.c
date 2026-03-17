#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "task.h"
#include "context.h"

void blink_task(void){
    while(1){
        PORTB ^= _BV(PB5);
        _delay_ms(1000);
    }
}

void slow_task(void){
    while(1){
        PORTB ^= _BV(PB5);
        _delay_ms(300);
    }
}

int main(void){
    DDRB |= _BV(PB5);
    DDRB |= _BV(PB4);

    // LED on PB5 - will stay on to indicate we're in main
    PORTB |= _BV(PB5);
    _delay_ms(500);
    PORTB &= ~_BV(PB5);
    _delay_ms(500);
    
    // Flash 1: Before RTOS init
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    rtos_init();
    
    // Flash 2: After RTOS init
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    tcbptr mainptr = init_main_tcb();
    
    // Flash 3: After main TCB init
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    tcbptr task1 = task_create(blink_task, 0);
    
    // Flash 4: After task1 created
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    tcbptr task2 = task_create(slow_task, 0);
    
    // Flash 5: After task2 created
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    current_tcb = mainptr;
    next_tcb = task1;
    
    // Flash 6: Before context switch
    PORTB |= _BV(PB5);
    _delay_ms(200);
    PORTB &= ~_BV(PB5);
    _delay_ms(200);
    
    context_switch();  // Should never return
    
    // If you see this, context_switch returned!
    while(1){
        PORTB ^= _BV(PB5);
        _delay_ms(100);
    }
}