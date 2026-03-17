#include "port.h"

TCB_t *pxCurrentTCB;

void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void)) {
    uint8_t *sp = stack + STACK_SIZE - 1;
    uint16_t addr = (uint16_t)task_func;

    *sp-- = (uint8_t)(addr & 0xFF);        // PC low
    *sp-- = (uint8_t)((addr >> 8) & 0xFF); // PC high

    *sp-- = 0;  // r0
    *sp-- = 0;  // SREG
    *sp-- = 0;  // r1
    *sp-- = 0;  // r2
    *sp-- = 0;  // r3
    *sp-- = 0;  // r4
    *sp-- = 0;  // r5
    *sp-- = 0;  // r6
    *sp-- = 0;  // r7
    *sp-- = 0;  // r8
    *sp-- = 0;  // r9
    *sp-- = 0;  // r10
    *sp-- = 0;  // r11
    *sp-- = 0;  // r12
    *sp-- = 0;  // r13
    *sp-- = 0;  // r14
    *sp-- = 0;  // r15
    *sp-- = 0;  // r16
    *sp-- = 0;  // r17
    *sp-- = 0;  // r18
    *sp-- = 0;  // r19
    *sp-- = 0;  // r20
    *sp-- = 0;  // r21
    *sp-- = 0;  // r22
    *sp-- = 0;  // r23
    *sp-- = 0;  // r24
    *sp-- = 0;  // r25
    *sp-- = 0;  // r26
    *sp-- = 0;  // r27
    *sp-- = 0;  // r28
    *sp-- = 0;  // r29
    *sp-- = 0;  // r30
    *sp-- = 0;  // r31

    tcb->pxTopOfStack = sp;
}