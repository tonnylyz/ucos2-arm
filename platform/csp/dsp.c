#include <snprintf.h>
#include "dsp.h"
#include "mmio.h"

#define DSP1_PRM_BASE           KADDR(0x4AE06400)

#define DSP1_MMU_IRQSTATUS      KADDR(0x40D01018)
#define DSP1_MMU_FAULT_PC       KADDR(0x40D01080)
#define DSP1_MMU_FAULT_AD       KADDR(0x40D01048)
#define DSP1_MMU_FAULT_STATUS   KADDR(0x40D01084)

void dsp1_mmu_debug() {
    printf("DSP1_MMU_IRQSTATUS %08x\n", mmio_read(DSP1_MMU_IRQSTATUS));
    printf("DSP1_MMU_FAULT_PC %08x\n", mmio_read(DSP1_MMU_FAULT_PC));
    printf("DSP1_MMU_FAULT_AD %08x\n", mmio_read(DSP1_MMU_FAULT_AD));
    printf("DSP1_MMU_FAULT_STATUS %08x\n", mmio_read(DSP1_MMU_FAULT_STATUS));

    while (1) {
        asm volatile ("nop");
    }
}