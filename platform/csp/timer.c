#include "timer.h"
#include "uart.h"
#include <types.h>


static inline u32 relaxed_mmio_read(u32 ptr) {
    u32 val;
    asm volatile("ldr %0, %1"
    : "=r" (val)
    : "Qo" (*(volatile u32 *) ptr));
    return val;
}

static inline void relaxed_mmio_write(u32 ptr, u32 value) {
    asm volatile("str %1, %0"
    : : "Qo" (*(volatile u32 *) ptr), "r" (value));
}

static inline void relax_cpu() {
    asm volatile ("dsb; isb");
}

#define OMAP_TIMER_PENDING 0x4ae18048
#define OMAP_TIMER_FUNC_BASE 0x4ae18014

static inline void omap_dm_timer_write(u32 reg, u32 value) {
    while (relaxed_mmio_read(OMAP_TIMER_PENDING) & (reg >> 16)) {
        relax_cpu();
    }
    relaxed_mmio_write(OMAP_TIMER_FUNC_BASE + (reg & 0xff), value);
}

static inline u32 omap_dm_timer_read(u32 reg) {
    while (relaxed_mmio_read(OMAP_TIMER_PENDING) & (reg >> 16)) {
        relax_cpu();
    }
    return relaxed_mmio_read(OMAP_TIMER_FUNC_BASE + (reg & 0xff));
}

void timer_init() {
    int TIMER_FREQUENCY = 32768;

    omap_dm_timer_write(/* LOAD  */ 0x4002c, 0xffffffffU - TIMER_FREQUENCY);
    omap_dm_timer_write(/* COUNT */ 0x20028, 0xffffffffU - TIMER_FREQUENCY);
    omap_dm_timer_write(/* CTRL  */ 0x10024, /* Auto Reload + Start */ 0x3);

    uart_puts("READ OMAP DM TIMER COUNT\n");
    while (1) {
        uart_print_hex(omap_dm_timer_read(/* COUNT */ 0x20028));
        uart_puts("\n");
        int j = 0xfffff;
        while (j--) {
            asm volatile ("nop");
        }
    }

}