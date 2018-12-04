#ifndef UCOS2_UART_H
#define UCOS2_UART_H

#include <mmio.h>

#define UART_BASE_2 KADDR(0x4806c000U)
#define UART_BASE_3 KADDR(0x48020000U)
#define UART_BASE_8 KADDR(0x48422000U)

#include "types.h"

void uart_putc(char c);

void uart2_init();

void uart2_putc(char c);

void uart_puts(char *str);

void uart_print_dec(uint32_t dec);

void uart_print_bin(uint32_t bin);

void uart_print_hex(uint32_t hex);


#endif //UCOS2_UART_H
