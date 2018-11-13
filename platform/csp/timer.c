#include "timer.h"
#include "uart.h"
#include <types.h>


static inline u32 mmio_read(u32 ptr) {
    u32 val;
    asm volatile("ldr %0, %1"
    : "=r" (val)
    : "Qo" (*(volatile u32 *) ptr));
    return val;
}

static inline void mmio_write(u32 ptr, u32 value) {
    asm volatile("str %1, %0"
    : : "Qo" (*(volatile u32 *) ptr), "r" (value));
}

static inline void barrier() {
    asm volatile ("dsb; isb");
}

#define OMAP_TIMER_PENDING 0x4ae18048U
#define OMAP_TIMER_FUNC_BASE 0x4ae18014U

#define OMAP_TIMER_CTRL_PEND    0x1
#define OMAP_TIMER_COUNT_PEND   0x2
#define OMAP_TIMER_LOAD_PEND    0x4

#define OMAP_TIMER_IRQ_EN       0x18
#define OMAP_TIMER_WAKE_REG     0x20
#define OMAP_TIMER_CTRL_REG     0x24
#define OMAP_TIMER_COUNT_REG    0x28
#define OMAP_TIMER_LOAD_REG     0x2c

#define OMAP_TIMER_CTRL_VAL 0x3 /* Auto Reload + Start */


#define OMAP_TIMER_INT_CAPTURE			(1 << 2)
#define OMAP_TIMER_INT_OVERFLOW			(1 << 1)
#define OMAP_TIMER_INT_MATCH			(1 << 0)

static inline void omap_dm_timer_write(u32 reg, u32 pend, u32 value) {
    while (mmio_read(OMAP_TIMER_PENDING) & pend) {
        barrier();
    }
    mmio_write(OMAP_TIMER_FUNC_BASE + reg, value);
}

static inline u32 omap_dm_timer_read(u32 reg, u32 pend) {
    while (mmio_read(OMAP_TIMER_PENDING) & pend) {
        barrier();
    }
    return mmio_read(OMAP_TIMER_FUNC_BASE + reg);
}

void timer_init() {
    const int TIMER_FREQUENCY = 32768;

    unsigned int load_val = 0xffffffffU - TIMER_FREQUENCY;

    omap_dm_timer_write(OMAP_TIMER_LOAD_REG, OMAP_TIMER_LOAD_PEND, load_val);
    omap_dm_timer_write(OMAP_TIMER_COUNT_REG, OMAP_TIMER_COUNT_PEND, load_val);
    omap_dm_timer_write(OMAP_TIMER_CTRL_REG, OMAP_TIMER_CTRL_PEND, OMAP_TIMER_CTRL_VAL);

    omap_dm_timer_write(OMAP_TIMER_IRQ_EN, 0, OMAP_TIMER_INT_OVERFLOW);
    omap_dm_timer_write(OMAP_TIMER_WAKE_REG, 0, OMAP_TIMER_INT_OVERFLOW);

}

u32 timer_get_count() {
    return omap_dm_timer_read(OMAP_TIMER_COUNT_REG, OMAP_TIMER_COUNT_PEND);
}