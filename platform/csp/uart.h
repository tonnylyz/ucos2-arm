#ifndef UCOS2_UART_H
#define UCOS2_UART_H

#include "types.h"

void uart_init();

void uart_putc(char c);

void uart_puts(char *str);

void uart_print_dec(uint32_t dec);

void uart_print_bin(uint32_t bin);

void uart_print_hex(uint32_t hex);


#endif //UCOS2_UART_H
