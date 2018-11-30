#include <snprintf.h>
#include "uart.h"
#include <mmio.h>

#define UART_THR(base)  (base + 0x00)
#define UART_LSR(base)  (base + 0x14)

static void _uart_putc(u32 base, char c) {
    while ((mmio_readb(UART_LSR(base)) & 0x20) == 0);
    mmio_writeb(UART_THR(base), (u8)c);
}

void uart_putc(char c) {
    if (c == '\n') {
        _uart_putc(UART_BASE_3, '\r');
    }
    _uart_putc(UART_BASE_3, c);
}

void uart_puts(char *str) {
    char c;

    while (0 != (c = *str++)) {
        uart_putc(c);
    }
}

void uart_print_dec(uint32_t dec) {
    static uint32_t _depth = 0;
    if (dec == 0 && _depth == 0) {
        uart_putc('0');
        return;
    } else if (dec == 0 && _depth != 0) {
        _depth += 1;
        return;
    } else {
        _depth += 1;
        uart_print_dec(dec / 10);
        uart_putc((char) (dec % 10 + '0'));
    }
}

void uart_print_bin(uint32_t bin) {
    if (bin == 0) {
        uart_puts("0b");
        return;
    }
    uart_print_bin(bin / 2);
    uart_putc((char) (bin % 2 + '0'));
}

void uart_print_hex(uint32_t hex) {
    if (hex < 16) {
        uart_puts("0x");
        if (hex < 10)
            uart_putc(hex + '0');
        else
            uart_putc(hex - 10 + 'A');
    } else {
        uart_print_hex(hex / 16);
        hex %= 16;
        if (hex < 10)
            uart_putc(hex + '0');
        else
            uart_putc(hex - 10 + 'A');
    }
}
