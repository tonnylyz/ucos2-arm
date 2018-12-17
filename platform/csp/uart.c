#include <snprintf.h>
#include "uart.h"
#include <mmio.h>

#include <ti/csl/csl_uart.h>
#include <ti/csl/soc.h>

#define UC_UART_THR(base)  (base + 0x00)
#define UC_UART_LSR(base)  (base + 0x14)

static void _uart_putc(u32 base, char c) {
    while ((mmio_readb(UC_UART_LSR(base)) & 0x20) == 0);
    mmio_writeb(UC_UART_THR(base), (u8)c);
}

#define UART_MODULE_INPUT_CLK       (48000000U)
void _uart_init(uint32_t baseAddr, uint32_t baudRate,
                uint32_t wordLength,
                uint32_t stopBit, uint32_t parity,
                uint32_t mode) {
    uint32_t divisorValue = 0U, fifoConfig = 0U;

/* Performing a module reset. */
    UARTModuleReset(baseAddr);

/* Performing FIFO configurations. */
/*
** - Transmit Trigger Level Granularity is 4
** - Receiver Trigger Level Granularity is 1
** - Transmit FIFO Space Setting is 56. Hence TX Trigger level
**   is 8 (64 - 56). The TX FIFO size is 64 bytes.
** - The Receiver Trigger Level is 1.
** - Clear the Transmit FIFO.
** - Clear the Receiver FIFO.
** - DMA Mode enabling shall happen through SCR register.
** - DMA Mode 0 is enabled. DMA Mode 0 corresponds to No
**   DMA Mode. Effectively DMA Mode is disabled.
*/
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_4,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  UART_FCR_TX_TRIG_LVL_56,
                                  1,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

/* Configuring the FIFO settings. */
    UARTFIFOConfig(baseAddr, fifoConfig);

/* Performing Baud Rate settings. */
/* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                         baudRate,
                                         mode,
                                         UART_MIR_OVERSAMPLING_RATE_42);
/* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(baseAddr, divisorValue);

/* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

/* Programming the Line Characteristics. */
    UARTLineCharacConfig(baseAddr, (wordLength | stopBit), parity);

/* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(baseAddr);

/* Disabling Break Control. */
    UARTBreakCtl(baseAddr, UART_BREAK_COND_DISABLE);

/* Uart enable */
    UARTOperatingModeSelect(baseAddr, mode);
}

void uart1_init() {
    _uart_init(UART_BASE_1, 115200, UART_FRAME_WORD_LENGTH_8, UART_FRAME_NUM_STB_1, UART_PARITY_NONE, UART16x_OPER_MODE);
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

