#ifndef UCOS2_UART_H
#define UCOS2_UART_H

#define UART_BASE_2 0x4806c000U
#define UART_BASE_3 0x48020000U
#define UART_BASE_8 0x48422000U

#include "types.h"

void uart_init(u32 base);

void uart_putc(char c);

void uart_puts(char *str);

void uart_print_dec(uint32_t dec);

void uart_print_bin(uint32_t bin);

void uart_print_hex(uint32_t hex);


#endif //UCOS2_UART_H
