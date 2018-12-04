#include "gic.h"
#include "types.h"
#include "mmio.h"
#include "timer.h"

#include <ti/csl/csl_a15.h>
#include <ti/csl/arch/a15/csl_a15_startup.h>
#include <ti/csl/soc.h>
#include <ti/csl/soc/am572x/src/cslr_soc.h>
#include <ti/csl/soc/am572x/src/cslr_soc_aliases.h>
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

const int TIMER2_IRQ_ID = 70;

void irq_init() {
    CSL_armGicEnableIntr(&gCpuIntrf, TIMER2_IRQ_ID);
    //mmio_write(0x4a002a4c, 0); // crossbar setting: disable firewall error irq
}

void OS_CPU_IntHandler(u32 src_id) {
    uint32_t irq_num = IntGetPendingIntNum();
    if (irq_num != TIMER2_IRQ_ID) {
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
