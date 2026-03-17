#include "rtos.h"

void rtos_create_task(TCB *tcb, void (*task)(void), uint8_t *stack, uint16_t size)
{
    uint8_t *sp = stack + size - 1;

    uint16_t addr = (uint16_t)task;

    *sp-- = addr & 0xFF;
    *sp-- = addr >> 8;

    *sp-- = 0x80;  // SREG with interrupts enabled

    for (int i = 0; i < 32; i++)
        *sp-- = 0;

    tcb->sp = sp;
}