#include <cpu.h>
#include <types.h>

#define BSP_SDRAM_PHYS_BASE                  0UL
#define BSP_SDRAM_VIRT_BASE                  0UL
#define BSP_SDRAM_SIZE                      (1024 * 1024 * 1024)

#define BSP_MEM_STRONGLY_ORDERED             0x0
#define BSP_MEM_SHAREABLE_DEVICE             0x1
#define BSP_MEM_OIWB_NWA                     0x2
#define BSP_MEM_OIWT_NWA                     0x3
#define BSP_MEM_OINC                         0x4
#define BSP_MEM_OIWB_WA                      0x7
#define BSP_MEM_NON_SHAREABLE_DEVICE         0x8
#define BSP_MEM_CACHE_TYPE(outer, inner)    (0x10 | ((outer)<<2) | (inner))
#define BSP_MEM__NC                            0
#define BSP_MEM_WBWA                           1
#define BSP_MEM_WTNWA                          2
#define BSP_MEM_WBNWA                          3

#define BSP_AP_PNA_UNA                         0
#define BSP_AP_PRW_UNA                         1
#define BSP_AP_PRW_URO                         2
#define BSP_AP_PRW_URW                         3
#define BSP_AP_PRO_UNA                         5
#define BSP_AP_PRO_URO                         7

#define BSP_PAGETABLE_L1_SECT_SIZE           0x100000


#define BSP_PAGETABLE_L1_FAULT(id) LONG(id<<2)


#define BSP_PAGETABLE_L1_PGTBL(addr, dom, ns) \
	(((addr) << 10) | (((dom) & 0xf)<<5) | (((ns) & 1)<<3)  | 0x1)


#define BSP_PAGETABLE_L1_SECT(addr, mem_type , xn, ns, dom, ap, s, ng) \
	(((addr) & ~((1<< 20)-1)) | (((ns) & 1)<<19) | (((ng) & 1)<<17) | \
	(((s) & 1)<<16) | (((ap) & 0x4)<<15) | (((mem_type) & 0x1c)<<10) |\
	(((ap) & 3)<< 10) | (((dom) & 0xf)<<5) | (((mem_type) & 3)<<2) | 0x2)


#define BSP_PAGETABLE_L1_SUPSECT(addr, type, xn, ns, dom, ap, s, ng)	\
	((((addr) & ~0xFF) << 24) | (((ns) & 1)<<19) | (1<<18) |	\
	(((ng) & 1)<<17) | (((s) & 1)<<16) | (((ap) & 1)<<15) |\
	(((tex) & 0x7)<<12) | (((ap) & 3)<< 10) | (((mem_type) & 3)<<2) | 0x2)

#define BSP_PAGETABLE_PHYS_TO_VIRT(addr) \
	((u32)(addr) - (BSP_SDRAM_PHYS_BASE) + (BSP_SDRAM_VIRT_BASE))

#define BSP_PAGETABLE_VIRT_TO_PHYS(addr) \
        ((u32)(addr) - (BSP_SDRAM_VIRT_BASE) + (BSP_SDRAM_PHYS_BASE))

#define BSP_PAGETABLE_VADDR_TO_L1_INDEX(addr) ((addr)>> 20)


static u32 BSP_PageTable[4096] __attribute__ ((aligned (16*1024)));

void page_table_init() {
    u32 vaddr;
    u32 paddr;
    u32 *pt_phys = (u32 *) BSP_PageTable;

    for (paddr = BSP_SDRAM_PHYS_BASE, vaddr = BSP_SDRAM_VIRT_BASE;
         vaddr < 0xfff00000; vaddr += BSP_PAGETABLE_L1_SECT_SIZE, paddr += BSP_PAGETABLE_L1_SECT_SIZE) {
        pt_phys[BSP_PAGETABLE_VADDR_TO_L1_INDEX(vaddr)] = paddr | (3 << 10) | (0xf << 5) | 2;
    }

    asm volatile ("mcr p15, 0, %0, c2, c0, 0": : "r"((unsigned long) pt_phys));
    asm volatile ("mcr p15, 0, %0, c2, c0, 1": : "r"((unsigned long) pt_phys));
    u32 ttbcr;
    asm volatile ("mrc p15, 0, %0, c2, c0, 2":"=r"(ttbcr));
    ttbcr &= ~0b111;
    ttbcr |= 0b001; // 2G / 2G
    ttbcr &= ~(1 << 5);
    ttbcr &= ~(1 << 4);
    asm volatile ("mcr p15, 0, %0, c2, c0, 2": :"r"(ttbcr));

    asm volatile ("mcr p15, 0, %0, c3, c0, 0": :"r"(~0) : "memory");
    asm volatile ("mcr p15, 0, %0, c8, c7, 0": :"r"(0) : "memory");
    asm volatile ("mcr p15, 0, %0, c7, c5, 1": :"r"(0) : "memory");

    u32 sctlr;
    asm volatile ("mrc p15, 0, %0, c1, c0, 0":"=r"(sctlr));
    sctlr |= 0x40180d;
    sctlr &= ~(1 << 13);
    asm volatile ("mcr p15, 0, %0, c1, c0, 0": :"r"(sctlr) : "memory");
    asm volatile ("mov r0, r0; mov r0, r0;mov r0, r0; dsb; isb;");
}