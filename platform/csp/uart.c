#include "uart.h"

#define REG_THR  0x00  /* Transmitter holding reg. */
#define REG_RDR  0x00  /* Receiver data reg.       */
#define REG_BRDL 0x00  /* Baud rate divisor (LSB)  */
#define REG_BRDH 0x01  /* Baud rate divisor (MSB)  */
#define REG_IER  0x01  /* Interrupt enable reg.    */
#define REG_IIR  0x02  /* Interrupt ID reg.        */
#define REG_FCR  0x02  /* FIFO control reg.        */
#define REG_LCR  0x03  /* Line control reg.        */
#define REG_MDC  0x04  /* Modem control reg.       */
#define REG_LSR  0x05  /* Line status reg.         */
#define REG_MSR  0x06  /* Modem status reg.        */

/* equates for interrupt enable register */

#define IER_RXRDY 0x01 /* receiver data ready     */
#define IER_TBE 0x02   /* transmit bit enable     */
#define IER_LSR 0x04   /* line status interrupts  */
#define IER_MSI 0x08   /* modem status interrupts */

/* equates for interrupt identification register */

#define IIR_IP 0x01    /* interrupt pending bit           */
#define IIR_MASK 0x07  /* interrupt id bits mask          */
#define IIR_MSTAT 0x00 /* modem status interrupt          */
#define IIR_THRE 0X02  /* transmit holding register empty */
#define IIR_RBRF 0x04  /* receiver buffer register full   */
#define IIR_ID 0x06    /* interupt ID mask without IP     */
#define IIR_SEOB 0x06  /* serialization error or break    */

/* equates for FIFO control register */

#define FCR_FIFO 0x01    /* enable XMIT and RCVR FIFO */
#define FCR_RCVRCLR 0x02 /* clear RCVR FIFO           */
#define FCR_XMITCLR 0x04 /* clear XMIT FIFO           */

/*
 * Per PC16550D (Literature Number: SNLS378B):
 *
 * RXRDY, Mode 0: When in the 16450 Mode (FCR0 = 0) or in
 * the FIFO Mode (FCR0 = 1, FCR3 = 0) and there is at least 1
 * character in the RCVR FIFO or RCVR holding register, the
 * RXRDY pin (29) will be low active. Once it is activated the
 * RXRDY pin will go inactive when there are no more charac-
 * ters in the FIFO or holding register.
 *
 * RXRDY, Mode 1: In the FIFO Mode (FCR0 = 1) when the
 * FCR3 = 1 and the trigger level or the timeout has been
 * reached, the RXRDY pin will go low active. Once it is acti-
 * vated it will go inactive when there are no more characters
 * in the FIFO or holding register.
 *
 * TXRDY, Mode 0: In the 16450 Mode (FCR0 = 0) or in the
 * FIFO Mode (FCR0 = 1, FCR3 = 0) and there are no charac-
 * ters in the XMIT FIFO or XMIT holding register, the TXRDY
 * pin (24) will be low active. Once it is activated the TXRDY
 * pin will go inactive after the first character is loaded into the
 * XMIT FIFO or holding register.
 *
 * TXRDY, Mode 1: In the FIFO Mode (FCR0 = 1) when
 * FCR3 = 1 and there are no characters in the XMIT FIFO, the
 * TXRDY pin will go low active. This pin will become inactive
 * when the XMIT FIFO is completely full.
 */
#define FCR_MODE0 0x00 /* set receiver in mode 0 */
#define FCR_MODE1 0x08 /* set receiver in mode 1 */

/* RCVR FIFO interrupt levels: trigger interrupt with this bytes in FIFO */
#define FCR_FIFO_1  0x00 /* 1 byte in RCVR FIFO   */
#define FCR_FIFO_4  0x40 /* 4 bytes in RCVR FIFO  */
#define FCR_FIFO_8  0x80 /* 8 bytes in RCVR FIFO  */
#define FCR_FIFO_14 0xC0 /* 14 bytes in RCVR FIFO */

/* constants for line control register */

#define LCR_CS5   0x00 /* 5 bits data size             */
#define LCR_CS6   0x01 /* 6 bits data size             */
#define LCR_CS7   0x02 /* 7 bits data size             */
#define LCR_CS8   0x03 /* 8 bits data size             */
#define LCR_2_STB 0x04 /* 2 stop bits                  */
#define LCR_1_STB 0x00 /* 1 stop bit                   */
#define LCR_PEN   0x08 /* parity enable                */
#define LCR_PDIS  0x00 /* parity disable               */
#define LCR_EPS   0x10 /* even parity select           */
#define LCR_SP    0x20 /* stick parity select          */
#define LCR_SBRK  0x40 /* break control bit            */
#define LCR_DLAB  0x80 /* divisor latch access enable  */

/* constants for the modem control register */

#define MCR_DTR  0x01 /* dtr output               */
#define MCR_RTS  0x02 /* rts output               */
#define MCR_OUT1 0x04 /* output #1                */
#define MCR_OUT2 0x08 /* output #2                */
#define MCR_LOOP 0x10 /* loop back                */
#define MCR_AFCE 0x20 /* auto flow control enable */

/* constants for line status register */

#define LSR_RXRDY 0x01 /* receiver data available         */
#define LSR_OE    0x02 /* overrun error                   */
#define LSR_PE    0x04 /* parity error                    */
#define LSR_FE    0x08 /* framing error                   */
#define LSR_BI    0x10 /* break interrupt                 */
#define LSR_THRE  0x20 /* transmit holding register empty */
#define LSR_TEMT  0x40 /* transmitter empty               */

/* constants for modem status register */

#define MSR_DCTS  0x01 /* cts change                */
#define MSR_DDSR  0x02 /* dsr change                */
#define MSR_DRI   0x04 /* ring change               */
#define MSR_DDCD  0x08 /* data carrier change       */
#define MSR_CTS   0x10 /* complement of cts         */
#define MSR_DSR   0x20 /* complement of dsr         */
#define MSR_RI    0x40 /* complement of ring signal */
#define MSR_DCD   0x80 /* complement of dcd         */

/* convenience defines */
#define UART_REG_ADDR_INTERVAL 4

#define THR(n)  (n + REG_THR * UART_REG_ADDR_INTERVAL)
#define RDR(n)  (n + REG_RDR * UART_REG_ADDR_INTERVAL)
#define BRDL(n) (n + REG_BRDL * UART_REG_ADDR_INTERVAL)
#define BRDH(n) (n + REG_BRDH * UART_REG_ADDR_INTERVAL)
#define IER(n)  (n + REG_IER * UART_REG_ADDR_INTERVAL)
#define IIR(n)  (n + REG_IIR * UART_REG_ADDR_INTERVAL)
#define FCR(n)  (n + REG_FCR * UART_REG_ADDR_INTERVAL)
#define LCR(n)  (n + REG_LCR * UART_REG_ADDR_INTERVAL)
#define MDC(n)  (n + REG_MDC * UART_REG_ADDR_INTERVAL)
#define LSR(n)  (n + REG_LSR * UART_REG_ADDR_INTERVAL)
#define MSR(n)  (n + REG_MSR * UART_REG_ADDR_INTERVAL)

#define IIRC(n) uart[n].iirCache

#define INBYTE(x) (*(volatile unsigned char *)(x))
#define OUTBYTE(x, d) do { *(volatile unsigned char *)(x) = (d); } while (0)


#define DIV_ROUND_CLOSEST(x, divisor)(            \
{                            \
    typeof(x) __x = x;                \
    typeof(divisor) __d = divisor;            \
    (((typeof(x))-1) > 0 ||                \
     ((typeof(divisor))-1) > 0 || (__x) > 0) ?    \
        (((__x) + ((__d) / 2)) / (__d)) :    \
        (((__x) - ((__d) / 2)) / (__d));    \
}                            \
)

void uart_init(u32 base) {

    int baud_rate = 115200;
    uint32_t divisor = 0; /* baud rate divisor */

    while (!(INBYTE(LSR(base)) & 0x40));
    OUTBYTE(IER(base), 0x00);

    OUTBYTE(BRDL(base), (unsigned char) (divisor & 0xff));
    OUTBYTE(BRDH(base), (unsigned char) ((divisor >> 8) & 0xff));
    OUTBYTE(LCR(base), LCR_DLAB | LCR_CS8 | LCR_1_STB | LCR_PDIS);

    /* calculate baud rate divisor */
    //divisor = (48000000 / 16 / baud_rate);
    divisor = DIV_ROUND_CLOSEST(48000000, 16 * baud_rate);

    /* set the DLAB to access the baud rate divisor registers */

    OUTBYTE(MDC(base), 0x3);
    OUTBYTE(FCR(base), 0x7);
    OUTBYTE(LCR(base), LCR_DLAB | LCR_CS8 | LCR_1_STB | LCR_PDIS);
    OUTBYTE(BRDL(base), (unsigned char) (divisor & 0xff));
    OUTBYTE(BRDH(base), (unsigned char) ((divisor >> 8) & 0xff));

    /* 8 data bits, 1 stop bit, no parity, clear DLAB */
    OUTBYTE(LCR(base), LCR_CS8 | LCR_1_STB | LCR_PDIS);
}

static void _uart_putc(u32 base, char c) {
    while ((INBYTE(LSR(base)) & 0x20) == 0);
    OUTBYTE(THR(base), c);
}

void uart_putc(char c) {
    if (c == '\n') {
        _uart_putc(UART_BASE_2, '\r');
        //_uart_putc(UART_BASE_2, '\r');
    }
    _uart_putc(UART_BASE_2, c);
    //_uart_putc(UART_BASE_2, c);
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
