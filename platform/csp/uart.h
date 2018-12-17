#ifndef UCOS2_UART_H
#define UCOS2_UART_H

#include <mmio.h>

#define UART_BASE_1 KADDR(0x4806a000U)
#define UART_BASE_2 KADDR(0x4806c000U)
#define UART_BASE_3 KADDR(0x48020000U)
#define UART_BASE_8 KADDR(0x48422000U)

#include "types.h"

void uart1_init();

void uart_putc(char c);

void uart_puts(char *str);

#endif //UCOS2_UART_H
