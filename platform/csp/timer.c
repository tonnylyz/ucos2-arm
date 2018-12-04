#include "timer.h"
#include "uart.h"
#include <types.h>
#include <mmio.h>

#define GP_TIMER_BASE         KADDR(0x48032000u) //Timer 2

#define GP_TIMER_TWPS      (GP_TIMER_BASE + 0x48u)

#define GP_TIMER_TCLR_PEND    0x1
#define GP_TIMER_TCRR_PEND    0x2
#define GP_TIMER_TLDR_PEND    0x4

#define GP_TIMER_IRQSTATUS_RAW  (GP_TIMER_BASE + 0x24u)
#define GP_TIMER_IRQSTATUS      (GP_TIMER_BASE + 0x28u)
#define GP_TIMER_IRQSTATUS_SET  (GP_TIMER_BASE + 0x2Cu)
#define GP_TIMER_TCLR           (GP_TIMER_BASE + 0x38u)
#define GP_TIMER_TCRR           (GP_TIMER_BASE + 0x3Cu)
#define GP_TIMER_TLDR           (GP_TIMER_BASE + 0x40u)

#define GP_TIMER_TCLR_VAL_AR_ST     0x3 /* Auto Reload + Start */
#define GP_TIMER_TCLR_VAL_ST        0x1 /* Start */


#define OMAP_TIMER_INT_CAPTURE			(1 << 2)
#define OMAP_TIMER_INT_OVERFLOW			(1 << 1)
#define OMAP_TIMER_INT_MATCH			(1 << 0)

static inline void pend_write(u32 reg, u32 pend, u32 value) {
    while (mmio_read(GP_TIMER_TWPS) & pend) {
        barrier();
    }
    mmio_write(reg, value);
}

static inline u32 pend_read(u32 reg, u32 pend) {
    while (mmio_read(GP_TIMER_TWPS) & pend) {
        barrier();
    }
    return mmio_read(reg);
}

void timer_init() {
    const int TIMER_FREQUENCY = 0x100000;

    unsigned int load_val = 0xffffffffU - TIMER_FREQUENCY;

    pend_write(GP_TIMER_TLDR, GP_TIMER_TLDR_PEND, load_val);
    pend_write(GP_TIMER_TCRR, GP_TIMER_TCRR_PEND, load_val);
    pend_write(GP_TIMER_TCLR, GP_TIMER_TCLR_PEND, GP_TIMER_TCLR_VAL_AR_ST);

    pend_write(GP_TIMER_IRQSTATUS_SET, 0, OMAP_TIMER_INT_OVERFLOW);
}

u32 timer_get_count() {
    return pend_read(GP_TIMER_TCRR, GP_TIMER_TCRR_PEND);
}

u32 timer_get_status() {
    return mmio_read(GP_TIMER_IRQSTATUS);
}

void timer_clear_irq() {
    mmio_write(GP_TIMER_IRQSTATUS, 0b10);
}
