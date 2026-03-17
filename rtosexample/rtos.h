#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

typedef struct {
    uint8_t *sp;
} TCB;

void rtos_create_task(TCB *tcb, void (*task)(void), uint8_t *stack, uint16_t size);
void rtos_switch(TCB *current, TCB *next) __attribute__((naked));
#endif