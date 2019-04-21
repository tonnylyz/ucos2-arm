#include <snprintf.h>
#include "dsp.h"
#include "mmio.h"

#define DSP1_PRM_BASE           KADDR(0x4AE06400)

#define DSP1_MMU_IRQSTATUS      KADDR(0x40D01018)
#define DSP1_MMU_FAULT_PC       KADDR(0x40D01080)
#define DSP1_MMU_FAULT_AD       KADDR(0x40D01048)
#define DSP1_MMU_FAULT_STATUS   KADDR(0x40D01084)

#define DSP1_BOOTADDR           KADDR(0x4A00255C)
#define DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK	(0x3FFFFF << 0)

#define DSP1_L1_PGTABLE         KADDR(0xbfc10000)
#define DSP2_L1_PGTABLE         KADDR(0xbfc18000)

#define MMU_DESCRIPTOR_1M  (0x00000002)
#define MMU_DESCRIPTOR_16M (0x00040002)

void dsp_mem_map_1m(u32 pgtable_base, u32 va, u32 pa) {
    mmio_write(KADDR(pgtable_base) + va / 0x40000, pa | MMU_DESCRIPTOR_1M);
}

void dsp_mem_map_16m(u32 pgtable_base, u32 va, u32 pa) {
    int i;
    for (i = 0; i < 16; i++) {
        mmio_write(KADDR(pgtable_base) + va / 0x40000 + i * 4, pa | MMU_DESCRIPTOR_16M);
    }
}

static void dsp_pgtable_init() {
    /* Note:
     *  Directly map 0x9500_0000 ~ 0x9600_0000 (16MB) for both DSP 1/2
     *  DSP 1/2 use uniformed memory layout and same kernel image
     * */

    dsp_mem_map_16m(DSP1_L1_PGTABLE, 0x95000000, 0x95000000);

    dsp_mem_map_16m(DSP1_L1_PGTABLE, 0x4a000000, 0x4a000000);

    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48000000, 0x48000000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48100000, 0x48100000);

    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48400000, 0x48400000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48500000, 0x48500000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48600000, 0x48600000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48700000, 0x48700000);

    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48800000, 0x48800000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48900000, 0x48900000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48a00000, 0x48a00000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48b00000, 0x48b00000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48c00000, 0x48c00000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48d00000, 0x48d00000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48e00000, 0x48e00000);
    dsp_mem_map_1m(DSP1_L1_PGTABLE, 0x48f00000, 0x48f00000);


    dsp_mem_map_16m(DSP2_L1_PGTABLE, 0x95000000, 0x95000000);

    dsp_mem_map_16m(DSP2_L1_PGTABLE, 0x4a000000, 0x4a000000);

    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48000000, 0x48000000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48100000, 0x48100000);

    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48400000, 0x48400000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48500000, 0x48500000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48600000, 0x48600000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48700000, 0x48700000);

    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48800000, 0x48800000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48900000, 0x48900000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48a00000, 0x48a00000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48b00000, 0x48b00000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48c00000, 0x48c00000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48d00000, 0x48d00000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48e00000, 0x48e00000);
    dsp_mem_map_1m(DSP2_L1_PGTABLE, 0x48f00000, 0x48f00000);
}

void dsp1_start_core() {

    dsp_pgtable_init();
    u32 boot_reg;

    boot_reg = mmio_read(DSP1_BOOTADDR);
    boot_reg = (boot_reg & (~DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));
    boot_reg =
            (boot_reg |
             ((0x95000000 >> 10) &
              DRA7XX_CTRL_CORE_DSP_RST_VECT_MASK));

    mmio_write(DSP1_BOOTADDR, boot_reg);

    mmio_write(DSP1_PRM_BASE + 0x10, 0x0);
    while (((mmio_read(DSP1_PRM_BASE + 0x14) & 0x3) != 0x3));
}



void dsp1_mmu_debug() {
    printf("DSP1_MMU_IRQSTATUS %08x\n", mmio_read(DSP1_MMU_IRQSTATUS));
    printf("DSP1_MMU_FAULT_PC %08x\n", mmio_read(DSP1_MMU_FAULT_PC));
    printf("DSP1_MMU_FAULT_AD %08x\n", mmio_read(DSP1_MMU_FAULT_AD));
    printf("DSP1_MMU_FAULT_STATUS %08x\n", mmio_read(DSP1_MMU_FAULT_STATUS));
    while (1) {
        asm volatile ("nop");
    }
}