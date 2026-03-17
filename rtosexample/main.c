#include <avr/io.h>
#include "rtos.h"

#define STACK 128

TCB main_tcb;
TCB t1, t2;

uint8_t stack1[STACK];
uint8_t stack2[STACK];

void delay_short()
{
    for (volatile long i=0;i<60000;i++);
}
register uint8_t marker asm("r20");
void delay_long()
{
    for (volatile long i=0;i<200000;i++);
}

void task1()
{
    marker = 1;

    while (1)
    {
        if(marker == 1)
            PORTB |= (1<<PB5);
        else
            PORTB &= ~(1<<PB5);

        for (volatile long i=0;i<150000;i++);

        rtos_switch(&t1,&t2);
    }
}

void task2()
{
    marker = 2;

    while (1)
    {
        if(marker == 2)
            PORTB &= ~(1<<PB5);
        else
            PORTB |= (1<<PB5);

        for (volatile long i=0;i<150000;i++);

        rtos_switch(&t2,&t1);
    }
}

int main()
{
    DDRB |= (1<<PB5);

    rtos_create_task(&t1, task1, stack1, STACK);
    rtos_create_task(&t2, task2, stack2, STACK);

    rtos_switch(&main_tcb, &t1);

    while(1);
}