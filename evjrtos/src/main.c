#define UBRR_VAL 16
#include <avr/io.h>

void uart_init(uint16_t ubrr) {
    UBRR0H = (UBRR_VAL >> 8);
    UBRR0L = (UBRR_VAL & 0xFF);
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
// void uart_init(void) {
//     UBRR0H = (UBRR_VAL >> 8);
//     UBRR0L = (UBRR_VAL & 0xFF);
//     UCSR0A = (1 << U2X0);          // Double speed mode
//     UCSR0B = (1 << TXEN0);         // Enable TX only
//     UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit frame
// }

void uart_send_byte(uint8_t byte) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
    UDR0 = byte;
}

void uart_print_str(const char *str) {
    while (*str) {
        uart_send_byte((uint8_t)*str++);
    }
}

void uart_print_hex(uint8_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    uart_send_byte('0');
    uart_send_byte('x');
    uart_send_byte(hex_chars[(val >> 4) & 0xF]);
    uart_send_byte(hex_chars[val & 0xF]);
}

void uart_print_newline(void) {
    uart_send_byte('\r');
    uart_send_byte('\n');
}

typedef struct{
    uint8_t* pxTopOfStack;

} TCB_h;

typedef TCB_h* TCB;

extern TCB_h *pxCurrentTCB;

#define portSAVE_CONTEXT()                              \
    asm volatile (                                      \
        "push r0                    \n\t"               \
        "in r0, __SREG__            \n\t"               \
        "cli                        \n\t"               \
        "push r0                    \n\t"               \
        "push r1                    \n\t"               \
        "clr r1                     \n\t"               \
        "push r2                    \n\t"               \
        "push r3                    \n\t"               \
        "push r4                    \n\t"               \
        "push r5                    \n\t"               \
        "push r6                    \n\t"               \
        "push r7                    \n\t"               \
        "push r8                    \n\t"               \
        "push r9                    \n\t"               \
        "push r10                   \n\t"               \
        "push r11                   \n\t"               \
        "push r12                   \n\t"               \
        "push r13                   \n\t"               \
        "push r14                   \n\t"               \
        "push r15                   \n\t"               \
        "push r16                   \n\t"               \
        "push r17                   \n\t"               \
        "push r18                   \n\t"               \
        "push r19                   \n\t"               \
        "push r20                   \n\t"               \
        "push r21                   \n\t"               \
        "push r22                   \n\t"               \
        "push r23                   \n\t"               \
        "push r24                   \n\t"               \
        "push r25                   \n\t"               \
        "push r26                   \n\t"               \
        "push r27                   \n\t"               \
        "push r28                   \n\t"               \
        "push r29                   \n\t"               \
        "push r30                   \n\t"               \
        "push r31                   \n\t"               \
        "lds r26, pxCurrentTCB      \n\t"               \
        "lds r27, pxCurrentTCB + 1  \n\t"               \
        "in r0, __SP_L__            \n\t"               \
        "st x+, r0                  \n\t"               \
        "in r0, __SP_H__            \n\t"               \
        "st x+, r0                  \n\t"               \
    );

#define portRESTORE_CONTEXT()                           \
    asm volatile (                                      \
        "lds r26, pxCurrentTCB      \n\t"               \
        "lds r27, pxCurrentTCB + 1  \n\t"               \
        "ld r28, x+                 \n\t"               \
        "out __SP_L__, r28          \n\t"               \
        "ld r29, x+                 \n\t"               \
        "out __SP_H__, r29          \n\t"               \
        "pop r31                    \n\t"               \
        "pop r30                    \n\t"               \
        "pop r29                    \n\t"               \
        "pop r28                    \n\t"               \
        "pop r27                    \n\t"               \
        "pop r26                    \n\t"               \
        "pop r25                    \n\t"               \
        "pop r24                    \n\t"               \
        "pop r23                    \n\t"               \
        "pop r22                    \n\t"               \
        "pop r21                    \n\t"               \
        "pop r20                    \n\t"               \
        "pop r19                    \n\t"               \
        "pop r18                    \n\t"               \
        "pop r17                    \n\t"               \
        "pop r16                    \n\t"               \
        "pop r15                    \n\t"               \
        "pop r14                    \n\t"               \
        "pop r13                    \n\t"               \
        "pop r12                    \n\t"               \
        "pop r11                    \n\t"               \
        "pop r10                    \n\t"               \
        "pop r9                     \n\t"               \
        "pop r8                     \n\t"               \
        "pop r7                     \n\t"               \
        "pop r6                     \n\t"               \
        "pop r5                     \n\t"               \
        "pop r4                     \n\t"               \
        "pop r3                     \n\t"               \
        "pop r2                     \n\t"               \
        "pop r1                     \n\t"               \
        "pop r0                     \n\t"               \
        "out __SREG__, r0           \n\t"               \
        "pop r0                     \n\t"               \
    );

#define STACK_SIZE 128

static uint8_t task0_stack[STACK_SIZE];
static uint8_t task1_stack[STACK_SIZE];

static TCB_h task0_tcb;
static TCB_h task1_tcb;

TCB_h *pxCurrentTCB;

void task_create(TCB_h *tcb, uint8_t *stack, void (*task_func)(void)){
    uint8_t *sp = stack + STACK_SIZE - 1;
    uint16_t addr = (uint16_t)task_func;
    *sp-- = (uint8_t)(addr & 0xFF);         // PC low
    *sp-- = (uint8_t)((addr >> 8) & 0xFF);  // PC high
        *sp-- = 0;  // r0
    *sp-- = 0;  // SREG - interrupts disabled
    *sp-- = 0;  // r1 (gcc expects r1 = 0)
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

void task0(void) {
    while (1) {
        uart_print_str("Task 0\r\n");
        // switch to task1
        portSAVE_CONTEXT();
        pxCurrentTCB = &task1_tcb;
        
        portRESTORE_CONTEXT();
        asm volatile("ret");
    }
}

void task1(void) {
    while (1) {
        uart_print_str("Task 1\r\n");
        // switch back to task0
        portSAVE_CONTEXT();
        pxCurrentTCB = &task0_tcb;
        
        portRESTORE_CONTEXT();
        asm volatile("ret");
    }
}


int main(void) {
    uart_init(0);

    task_create(&task0_tcb, task0_stack, task0);
    task_create(&task1_tcb, task1_stack, task1);

    // Boot into task0
    pxCurrentTCB = &task0_tcb;
    portRESTORE_CONTEXT();
    asm volatile("ret");

    // Never reached
    while(1);
}