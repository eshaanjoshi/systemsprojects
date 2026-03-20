#include "port.h"
#include <avr/interrupt.h>
#include "scheduler.h"
#include "uart.h"

//void TIMER1_COMPA_vect(void) __attribute__((signal, naked));





uint8_t *pxPortInitialiseStack(uint8_t *pxTopOfStack, void (*pxCode)(void)) {
    uint16_t usAddress = (uint16_t)pxCode;

    // PC - low byte first
    *pxTopOfStack = (uint8_t)(usAddress & 0x00ff);
    pxTopOfStack--;
    usAddress >>= 8;
    *pxTopOfStack = (uint8_t)(usAddress & 0x00ff);
    pxTopOfStack--;

    // Simulate portSAVE_CONTEXT layout
    *pxTopOfStack = 0x00; // r0
    pxTopOfStack--;
    *pxTopOfStack = portFLAGS_INT_ENABLED; // SREG
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r1 - gcc expects zero
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r2
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r3
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r4
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r5
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r6
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r7
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r8
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r9
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r10
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r11
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r12
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r13
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r14
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r15
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r16
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r17
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r18
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r19
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r20
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r21
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r22
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r23
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r24
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r25
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r26
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r27
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r28
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r29
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r30
    pxTopOfStack--;
    *pxTopOfStack = 0x00; // r31
    pxTopOfStack--;

    return pxTopOfStack;
}



void prvSetupTimerInterrupt(void){
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A  = 15624; 
    TIMSK1 = (1 << OCIE1A);  
}



ISR(TIMER1_COMPA_vect, ISR_NAKED){
    vPortYieldFromTick();
    asm volatile("reti");
}

void vPortYieldFromTick(void){
    portSAVE_CONTEXT();
    //vTaskIncrementTick();
    tick_counter++;
    //uart_print_hex(tick_counter);
    unblock_if_finished();
    task_switch_context();
    portRESTORE_CONTEXT();
    asm volatile("ret");
}