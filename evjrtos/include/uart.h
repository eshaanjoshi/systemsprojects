#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_send_byte(uint8_t byte);
void uart_print_str(const char *str);
void uart_print_hex(uint8_t val);
void uart_print_newline(void);

#endif