#ifndef UCOS2_ARM_MMIO_H
#define UCOS2_ARM_MMIO_H

#include <types.h>

#define KADDR(_) ((_) | 0x80000000u)

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

static inline u8 mmio_readb(u32 ptr) {
    u8 val;
    asm volatile("ldrb %0, %1"
    : "=r" (val)
    : "Qo" (*(volatile u32 *) ptr));
    return val;
}

static inline void mmio_writeb(u32 ptr, u8 value) {
    asm volatile("strb %1, %0"
    : : "Qo" (*(volatile u32 *) ptr), "r" (value));
}

static inline void barrier() {
    asm volatile ("dsb; isb");
}

#endif //UCOS2_ARM_MMIO_H
