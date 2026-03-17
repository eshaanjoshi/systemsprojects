#include <avr/io.h>
#include "uart.h"

#define UBRR_VAL 16


void uart_init(void) {
    UBRR0H = (UBRR_VAL >> 8);
    UBRR0L = (UBRR_VAL & 0xFF);
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_byte(uint8_t byte) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = byte;
}

void uart_print_str(const char *str) {
    while (*str) uart_send_byte((uint8_t)*str++);
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
