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

static void dsp1_pgtable_init() {
//    carveout rsc: da 95000000, pa 0, len 100000, flags 0
//    alloc_mem: count 256 mask 0xff pageno 0x0
    mmio_write(KADDR(0xbfc12540), 0x99000002);
//    carveout mapped 0x95000000 to 0x99000000
//    carveout rsc: da 95100000, pa 0, len 100000, flags 0
//    alloc_mem: count 256 mask 0xff pageno 0x100
    mmio_write(KADDR(0xbfc12544), 0x99100002);
//    carveout mapped 0x95100000 to 0x99100000
//    devmem rsc: pa 0x4a000000, da 0x4a000000, len 0x1000000
    mmio_write(KADDR(0xbfc11280), 0x4a040002);
    mmio_write(KADDR(0xbfc11284), 0x4a040002);
    mmio_write(KADDR(0xbfc11288), 0x4a040002);
    mmio_write(KADDR(0xbfc1128c), 0x4a040002);
    mmio_write(KADDR(0xbfc11290), 0x4a040002);
    mmio_write(KADDR(0xbfc11294), 0x4a040002);
    mmio_write(KADDR(0xbfc11298), 0x4a040002);
    mmio_write(KADDR(0xbfc1129c), 0x4a040002);
    mmio_write(KADDR(0xbfc112a0), 0x4a040002);
    mmio_write(KADDR(0xbfc112a4), 0x4a040002);
    mmio_write(KADDR(0xbfc112a8), 0x4a040002);
    mmio_write(KADDR(0xbfc112ac), 0x4a040002);
    mmio_write(KADDR(0xbfc112b0), 0x4a040002);
    mmio_write(KADDR(0xbfc112b4), 0x4a040002);
    mmio_write(KADDR(0xbfc112b8), 0x4a040002);
    mmio_write(KADDR(0xbfc112bc), 0x4a040002);
//    mapped devmem pa 0x4a000000, da 0x4a000000, len 0x1000000
//    devmem rsc: pa 0x48000000, da 0x48000000, len 0x200000
    mmio_write(KADDR(0xbfc11200), 0x48000002);
    mmio_write(KADDR(0xbfc11204), 0x48100002);
//    mapped devmem pa 0x48000000, da 0x48000000, len 0x200000
//    devmem rsc: pa 0x48400000, da 0x48400000, len 0x400000
    mmio_write(KADDR(0xbfc11210), 0x48400002);
    mmio_write(KADDR(0xbfc11214), 0x48500002);
    mmio_write(KADDR(0xbfc11218), 0x48600002);
    mmio_write(KADDR(0xbfc1121c), 0x48700002);
//    mapped devmem pa 0x48400000, da 0x48400000, len 0x400000
//    devmem rsc: pa 0x48800000, da 0x48800000, len 0x800000
    mmio_write(KADDR(0xbfc11220), 0x48800002);
    mmio_write(KADDR(0xbfc11224), 0x48900002);
    mmio_write(KADDR(0xbfc11228), 0x48a00002);
    mmio_write(KADDR(0xbfc1122c), 0x48b00002);
    mmio_write(KADDR(0xbfc11230), 0x48c00002);
    mmio_write(KADDR(0xbfc11234), 0x48d00002);
    mmio_write(KADDR(0xbfc11238), 0x48e00002);
    mmio_write(KADDR(0xbfc1123c), 0x48f00002);
//    mapped devmem pa 0x48800000, da 0x48800000, len 0x800000
}

void dsp1_start_core() {

    dsp1_pgtable_init();
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

    printf("Dumping page table...\n");
    u32 *pgtable = (u32 *)DSP1_L1_PGTABLE;
    u32 i;
    for (i = 0; i < 8192; i++) {
        if (pgtable[i] != 0) {
            printf("[%08x] -> [%08x]\n", i, pgtable[i]);
        }
    }
    printf("page table dumped ok!\n");

    printf("Dumping context...\n");
    u32 *text = (u32 *)(0x99000000);
    for (i = 0; i < 128; i++) {
        if (text[i] != 0) {
            printf("[%08x] -> [%08x]\n", i, text[i]);
        }
    }
    printf("context dumped ok!\n");

    while (1) {
        asm volatile ("nop");
    }
}