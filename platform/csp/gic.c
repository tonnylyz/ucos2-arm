#include "gic.h"
#include "types.h"
#include "mmio.h"
#include "timer.h"
#include "dsp.h"

#include <ti/csl/arch/a15/csl_a15_startup.h>
#include <ti/csl/soc.h>
#include <snprintf.h>
#include <ucos_ii.h>

int32_t Osal_delay(uint32_t nTicks)
{
    printf("Osal_delay called.\n");
    while (1) {
        asm volatile ("nop");
    }
    return 0;
}

extern CSL_ArmGicDistIntrf distrIntrf;
extern CSL_ArmGicCpuIntrf gCpuIntrf;

static void CSL_armGicDefaultHandler(void* arg)
{
    /* Spurious interrupt could have happened
     * No action
     */
    return;
}

void gic_init() {

    gCpuIntrf.gicDist = &distrIntrf;
    gCpuIntrf.cpuIntfBasePtr = (void *)(KADDR(CSL_MPU_INTC_MPU_PHYS_CPU_IF_REGS));
    distrIntrf.distBasePtr = (void *)(KADDR(CSL_MPU_INTC_MPU_DISTRIBUTOR_REGS));
    gCpuIntrf.initStatus = (uint32_t)FALSE;
    gCpuIntrf.gicDist->initStatus = (uint32_t)FALSE;
    gCpuIntrf.pDefaultIntrHandlers = &CSL_armGicDefaultHandler;
    gCpuIntrf.pDefaultUserParameter = NULL;
    CSL_armGicInit(&gCpuIntrf);
}

#define IRQ_DSP1_MMU    60
#define IRQ_TIMER2      70

void irq_init() {
    u32 i;
    for (i = 32; i < 192; i++) {
        if (i == 40) continue;
        CSL_armGicEnableIntr(&gCpuIntrf, i);
    }
}

void OS_CPU_IntHandler(u32 src_id) {
    uint32_t irq_num = IntGetPendingIntNum();
    if (irq_num == IRQ_TIMER2) {

    } else if (irq_num == IRQ_DSP1_MMU) {
        dsp1_mmu_debug();
    } else {
        printf("Unexpected IRQ: %d\n", irq_num);
        while (1) {
            asm volatile ("nop");
        }
    }
    timer_clear_irq();
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}
