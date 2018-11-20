#include "types.h"
#include "gic.h"
#include "uart.h"

typedef uint32_t Uint32;
typedef uint8_t Uint8;
typedef uint32_t UInt32;

#define CSL_ARM_GIC_MAX_INTR_NO (1024u)
#define CSL_ARM_GIC_MAX_CPU_NO (8u)

#define CSL_ARM_GIC_CPU_INTF_MIN_PRI                ((uint32_t)0xffU)
#define CSL_ARM_GIC_CPU_INTF_MIN_PRI_ALL ((CSL_ARM_GIC_CPU_INTF_MIN_PRI) |       \
                                          ((CSL_ARM_GIC_CPU_INTF_MIN_PRI) << 8) |  \
                                          ((CSL_ARM_GIC_CPU_INTF_MIN_PRI) << 16) | \
                                          ((CSL_ARM_GIC_CPU_INTF_MIN_PRI) << 24))

#define CSL_ARM_GICD_DISABLE_INTR               ((uint32_t)0xFFFFFFFFU)

#define CSL_ARMGIC_GICD_CTLR_INTERRUPT_IN_EN_MASK               (0x00000001U)

#define SOC_INTC_MPU_DISTRIBUTOR_BASE (0x48211000u)
#define SOC_INTC_MPU_PHYS_CPU_IF_BASE (0x48212000u)

#define CSL_ARMGIC_GICD_SET_CLR_MASK(intrNum)                  ((uint32_t) 0x1 << ((intrNum) % 32U))
#define CSL_ARM_GICD_SET_CLR_MASK(intrNum)      (((uint32_t)0x1U) << ((intrNum) % 32U))

#define NUM_INTERRUPTS    (1024U)

static void CSL_armGicDefaultHandler(void* arg)
{
  /* Spurious interrupt could have happened
   * No action
   */
  return;
}

typedef enum
{
    CSL_ARM_GIC_INTR_NOT_SUPP            = 0U,
    CSL_ARM_GIC_INTR_SUPP                = 1U,
    CSL_ARM_GIC_INTR_SUPP_ENABLED_ALWAYS = 2U
} CSL_ArmGicIntrSupport_t;

static void IntDefaultHandler(void *dummy)
{
    /* Go into an infinite loop.*/
    volatile uint32_t loop = 1U;
    while (1U == loop)
    {
        ;
    }
}

typedef void (*IntrFuncPtr)(void *handle);

IntrFuncPtr    fnRAMVectors[NUM_INTERRUPTS];
void          *argArray[NUM_INTERRUPTS] = {0};

static void Intc_IntUnregister(uint16_t intrNum)
{
    uint32_t IntDefaultHandler_t = (uint32_t) (&IntDefaultHandler);
    /* Assign default ISR */
    fnRAMVectors[intrNum] = (IntrFuncPtr) IntDefaultHandler_t;
    argArray[intrNum]     = NULL;
}

typedef enum
{
    /**
     * @brief   Loads the GP Timer Period Register Low
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL0 = 0U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL1 = 1U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL2 = 2U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL3 = 3U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL4 = 4U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL5 = 5U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL6 = 6U,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   Uint32 *
     */
    CSL_ARM_GIC_BPVAL7 = 7U
} CSL_ArmGicBinPointVal_t;

typedef struct {
    volatile Uint32 GICD_CTLR;
    volatile Uint32 GICD_TYPER;
    volatile Uint32 GICD_IIDR;
    volatile Uint8  RSVD0[116];
    volatile Uint32 GICD_IGROUPR[16];
    volatile Uint8  RSVD1[64];
    volatile Uint32 GICD_ISENABLER[16];
    volatile Uint8  RSVD2[64];
    volatile Uint32 GICD_ICENABLER[16];
    volatile Uint8  RSVD3[64];
    volatile Uint32 GICD_ISPENDR[16];
    volatile Uint8  RSVD4[64];
    volatile Uint32 GICD_ICPENDR[16];
    volatile Uint8  RSVD5[64];
    volatile Uint32 GICD_ISACTIVER[16];
    volatile Uint8  RSVD6[64];
    volatile Uint32 GICD_ICACTIVER[16];
    volatile Uint8  RSVD7[64];
    volatile Uint32 GICD_IPRIORITYR[128];
    volatile Uint8  RSVD8[512];
    volatile Uint32 GICD_ITARGETSR[128];
    volatile Uint8  RSVD9[512];
    volatile Uint32 GICD_ICFGR[32];
    volatile Uint8  RSVD10[128];
    volatile Uint32 GICD_PPISR;
    volatile Uint32 GICD_SPISR[15];
    volatile Uint8  RSVD11[448];
    volatile Uint32 GICD_SGIR;
    volatile Uint8  RSVD12[12];
    volatile Uint32 GICD_CPENDSGIR[4];
    volatile Uint32 GICD_SPENDSGIR[4];
    volatile Uint8  RSVD13[160];
    volatile Uint32 GICD_PIDR4;
    volatile Uint32 GICD_PIDR5;
    volatile Uint32 GICD_PIDR6;
    volatile Uint32 GICD_PIDR7;
    volatile Uint32 GICD_PIDR0;
    volatile Uint32 GICD_PIDR1;
    volatile Uint32 GICD_PIDR2;
    volatile Uint32 GICD_PIDR3;
    volatile Uint32 GICD_CIDR[4];
} CSL_ArmgicDistributorRegs;

typedef struct {
    volatile Uint32 GICC_CTLR;
    volatile Uint32 GICC_PMR;
    volatile Uint32 GICC_BPR;
    volatile Uint32 GICC_IAR;
    volatile Uint32 GICC_EOIR;
    volatile Uint32 GICC_RPR;
    volatile Uint32 GICC_HPPIR;
    volatile Uint32 GICC_ABPR;
    volatile Uint32 GICC_AIAR;
    volatile Uint32 GICC_AEOIR;
    volatile Uint32 GICC_AHPPIR;
    volatile Uint8  RSVD0[164];
    volatile Uint32 GICC_APR0;
    volatile Uint8  RSVD1[12];
    volatile Uint32 GICC_NSAPR0;
    volatile Uint8  RSVD2[24];
    volatile Uint32 GICC_IIDR;
    volatile Uint8  RSVD3[3840];
    volatile Uint32 GICC_DIR;
} CSL_ArmgicGiccRegs;



typedef struct
{
    /** Pointer to distributor sub-module base address. */
    void *distBasePtr;

    /** GIC distributor initialization status. */
    Uint32 initStatus;

    /** Maximum number of interrupts supported. */
    Uint32 maxValidIntr;

    /** Number of CPU interfaces implemented in GIC. */
    Uint32 noCpuIntf;

    /** Number of priority levels implemented. */
    Uint32 noPriorityStep;

    /** NS binary point for NS priority group configuration. */
    CSL_ArmGicBinPointVal_t nonSecureBinaryPoint;

    /** Number of CPU interfaces implemented in GIC. */
    Uint32 intrSupportMask[CSL_ARM_GIC_MAX_INTR_NO / 32U];
} CSL_ArmGicDistIntrf;

typedef struct
{
    /** Pointer to CPU Interface sub-module base address. */
    void *cpuIntfBasePtr;
    /** CPU interfaces ID in GIC. */
    uint32_t cpuId;
    /** GIC CPU interface initialization status. */
    uint32_t initStatus;
    /** User configurable parameters. */
    CSL_ArmGicDistIntrf *gicDist;
    /**
     * Array of function pointers to hold call back functions for Interrupt
     * handlers. This list will be used for calling the call back functions
     * of interrupts.
     */
    void (*pDefaultIntrHandlers)(void* pUserParam);
    /**
     * Array to hold the user parameters passed during interrupt
     * registration. The same will be passed to the corresponding interrupt
     * handlers.
     */
    void *pDefaultUserParameter;
    /**
    * Interrupt ack register to read from Alias area or
    * non alias register for getting Interrupt ID from IAR register
    */
    uintptr_t         iarRegAddress;
    /**
    * End of Interrupt Register address to read from Alias or
    * non alias EOI register
    */
    uintptr_t         eoiRegAddress;
} CSL_ArmGicCpuIntrf;

CSL_ArmGicDistIntrf distrIntrf = {0};
CSL_ArmGicCpuIntrf gCpuIntrf = {0};

static inline CSL_ArmGicIntrSupport_t CSL_armGicIsIntrSupported(const CSL_ArmGicDistIntrf *gicInst, uint32_t intrNum)
{
    CSL_ArmgicDistributorRegs *gicDistInst = (CSL_ArmgicDistributorRegs *) gicInst->distBasePtr;

    CSL_ArmGicIntrSupport_t intrSuppResult = CSL_ARM_GIC_INTR_NOT_SUPP;
    uint32_t prevEnableConfig = 0;
    uint32_t prevCtrlConfig = 0;
    uint32_t temp;

    if (intrNum < ((uint32_t)CSL_ARM_GIC_MAX_INTR_NO >> 1))
    {

        /* Disable forwarding of interrupts in distributor */
        prevCtrlConfig = gicDistInst->GICD_CTLR;
        prevCtrlConfig &= (uint32_t)( ~(uint32_t)CSL_ARMGIC_GICD_CTLR_INTERRUPT_IN_EN_MASK);
        gicDistInst->GICD_CTLR =  prevCtrlConfig;

        /* Check if an interrupt is Supported or Not */
        prevEnableConfig = gicDistInst->GICD_ISENABLER[intrNum/32U];
        temp = (uint32_t) CSL_ARMGIC_GICD_SET_CLR_MASK(intrNum);
        gicDistInst->GICD_ISENABLER[intrNum/32U] = (Uint32) temp; /* ; */

        if ((gicDistInst->GICD_ISENABLER[intrNum/32U] != (Uint32) 0U) &&
            (CSL_ARMGIC_GICD_SET_CLR_MASK(intrNum) != (UInt32) 0U))
        {
            intrSuppResult = CSL_ARM_GIC_INTR_SUPP;

            /*  Check if the interrupt is permanently supported */
            gicDistInst->GICD_ICENABLER[intrNum/32U] = (Uint32) CSL_ARMGIC_GICD_SET_CLR_MASK(intrNum);

            if ((gicDistInst->GICD_ISENABLER[intrNum/32U] != (Uint32) 0U) &&
                (CSL_ARMGIC_GICD_SET_CLR_MASK(intrNum)  != (Uint32) 0U))
            {
                intrSuppResult = CSL_ARM_GIC_INTR_SUPP_ENABLED_ALWAYS;
            }
        }
        else
        {
            intrSuppResult = CSL_ARM_GIC_INTR_NOT_SUPP;
        }

        /* Clear all the interrupts in this register */
        gicDistInst->GICD_ICENABLER[intrNum/32U] = (Uint32) 0xFFFFFFFFU;

        /* Revert the original enable config, as it was while entering this
        function */
        gicDistInst->GICD_ISENABLER[intrNum/32U] = prevEnableConfig;

        /* Enable forwarding of interrupts in distributor */
        gicDistInst->GICD_CTLR =  prevCtrlConfig | (Uint32) CSL_ARMGIC_GICD_CTLR_INTERRUPT_IN_EN_MASK;
    }
    return intrSuppResult;
}

static inline void CSL_armGicInit(CSL_ArmGicCpuIntrf *cpuIntrf)
{
    CSL_ArmgicDistributorRegs *gicDistInst = (CSL_ArmgicDistributorRegs *) NULL;
    CSL_ArmgicGiccRegs *gicCpuInst = (CSL_ArmgicGiccRegs *) NULL;
    Uint32 gicDistIcType = 0;
    Uint32 maskedValue = 0;
    Uint32 intrNum = 0, i = 0, j = 0, intActiveReg = 0;
    Uint32 ctrlConfigval = 0;
    Uint32 PriorityStep = 0;
    Uint32 size, index;
#if defined(SOC_K2G) || defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2L) || defined(SOC_K2E)
    Bool   useAliasRegs;
#endif
#if defined(SOC_K2G)
    Uint32 secureSystStatus;
#endif

    gicCpuInst = (CSL_ArmgicGiccRegs *)(cpuIntrf->cpuIntfBasePtr);
    gicDistInst = (CSL_ArmgicDistributorRegs *)(cpuIntrf->gicDist->distBasePtr);

    if(FALSE == cpuIntrf->gicDist->initStatus)
    {
        cpuIntrf->cpuId = 0U;

        /* Set binary point register */
        /* TODO: Need to check this value
        ** cpuIntrf->gicConfig.nonSecureBinaryPoint = GicNsBinaryPointGet();
        */
        cpuIntrf->gicDist->nonSecureBinaryPoint = 0x03;

        /* Initialize GIC instance */
        gicDistIcType = gicDistInst->GICD_TYPER;

        /*  Get the number of interrupt lines supported */
        maskedValue = gicDistIcType & 0x1fU;
        /*  From the mapping get the exact number of interrupts supported */
        maskedValue = (maskedValue + 1U) * 32U;
        /*  Check for max interrupts */
        if (maskedValue > 1020U)
        {
            maskedValue = 1020U;
        }
        cpuIntrf->gicDist->maxValidIntr = maskedValue;

        /* Get the number of CPU interfaces implemented */
        maskedValue = ((gicDistIcType & 0xe0U) >> 5U) + 1U;
        /* Check for max no of CPU interface */
        if (maskedValue > CSL_ARM_GIC_MAX_CPU_NO)
        {
            maskedValue = CSL_ARM_GIC_MAX_CPU_NO;
        }
        cpuIntrf->gicDist->noCpuIntf = maskedValue;

        /* Find the priority levels implemented */
        for(intrNum = 0; intrNum < ((Uint32)CSL_ARM_GIC_MAX_INTR_NO >> 1); intrNum++)
        {
            if ((CSL_armGicIsIntrSupported(cpuIntrf->gicDist, intrNum)) != CSL_ARM_GIC_INTR_NOT_SUPP)
            {
                index = intrNum & 3u;
                PriorityStep = (UInt32) 0xFFu;
                gicDistInst->GICD_IPRIORITYR[index] |= (PriorityStep << (8u*index));

                /* Extract the priority level */
                cpuIntrf->gicDist->noPriorityStep = ((~PriorityStep) & 0x0FU) + 1U;
                break;
            }
        }

        /* Register the default handler for all interrupts */
        for(intrNum = 0; intrNum < cpuIntrf->gicDist->maxValidIntr; intrNum++)
        {
          /*
           * The Aux layer is used for OSAL AM335x/AM437x
           * Since the interrupt implementation for A15
           * is done in CSL lib which does not exist
           * for AM335x and AM437x, we keep the original
           * CSL implementation
           */
            Intc_IntUnregister((uint16_t)intrNum);


#if defined(SOC_K2G) || defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2L) || defined(SOC_K2E)
         /* initialize the GIC, registers to enable group1 IRQs
          * this is in line with the GEL file
          * please update the priority registers if interrupts are required to be
          * set for Group 0 registers
          */
          gicDistInst->GICD_IGROUPR[intrNum/32U] = 0xFFFFFFFFU;
#endif
        }

        /*
           Register handlers for interrupt ID's 1020-1023. GIC will implement these
           interrupts irrespective of the number of interrupts supported in the device.
        */
        for(intrNum = 1020U; intrNum < 1024U; intrNum++)
        {
          /*
           * The Aux layer is used for OSAL AM335x/AM437x
           * Since the interrupt implementation for A15
           * is done in CSL lib which does not exist
           * for AM335x and AM437x, we keep the original
           * CSL implementation
           */
          Intc_IntUnregister((uint16_t)intrNum);

        }

        /*  Initialize distributor sub-module */

        /*  Disable distributor sub-module */
        gicDistInst->GICD_CTLR = 0x0U;

        /*  Initialize all SPI interrupts */

        /*  Configure priority on all SPI interrupts to the lowest priority */
        size = sizeof(gicDistInst->GICD_IPRIORITYR) >> 2U;
        for (intrNum = 32U; intrNum < cpuIntrf->gicDist->maxValidIntr; intrNum += 1U)
        {
           /* limitting to the arrary size of 128 to handle upto 512
            * interrupt lines */
            if ((intrNum) < size)
            {
                gicDistInst->GICD_IPRIORITYR[intrNum] = (Uint32)CSL_ARM_GIC_CPU_INTF_MIN_PRI_ALL;
            }
            else
            {
                /* Break from the loop */
                break;
            }
        }

        /*  Disable all SPI interrupts */
        for (intrNum = 32U; intrNum < cpuIntrf->gicDist->maxValidIntr; intrNum += 32U)
        {
            if((intrNum / 32U) < 16U)
            {
                gicDistInst->GICD_ICENABLER[intrNum / 32U] = CSL_ARM_GICD_DISABLE_INTR;
            }
        }

        /*  Enable distributor sub-module */
#if defined(SOC_K2G) || defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2L) || defined(SOC_K2E)
        /* Enable Group1 and Group0 Interrupts */
        gicDistInst->GICD_CTLR = 0x3U;
#else
        gicDistInst->GICD_CTLR = 0x1U;
#endif
        /* Search for any previously active interrupts and acknowledge them */
        for (i = 0U; i < 6U; i++)
        {
            intActiveReg = gicDistInst->GICD_ISACTIVER[i];

            if (intActiveReg)
            {
                for (j = 0U; j < 32U; j++)
                {
                    if (intActiveReg & 0x1U)
                    {
                        gicCpuInst->GICC_EOIR = ((i * 32U) + j);
                    }
                    intActiveReg = intActiveReg >> 1;
                }
            }
        }

        /* GICv2 SGIC Clear Pending Register */
        for(i = 0; i < 4U; i++)
        {
            gicDistInst->GICD_CPENDSGIR[i] = 0x01010101U;
        }

        /* Clear all interrupt active status registers. */
        for(i = 0; i < 6U; i++)
        {
             gicDistInst->GICD_ICPENDR[i] = 0xFFFFFFFFU;
        }

        /* Clear active register */
         for(i = 0; i < 6U; i++)
        {
             gicDistInst->GICD_ICACTIVER[i] = 0xFFFFFFFFU;
        }

        /* Target processor register */
        size = sizeof(gicDistInst->GICD_ITARGETSR) >> 2U;
        for(intrNum = 8U; intrNum < cpuIntrf->gicDist->maxValidIntr; intrNum++)
        {
            /* limitting to the arrary size of 128 to handle upto 512
             * interrupt lines */
            if (intrNum < size)
            {
                gicDistInst->GICD_ITARGETSR[intrNum] = 0x03030303U;
            }
            else
            {
                /* Break from the loop */
                break;
            }
        }

        cpuIntrf->gicDist->initStatus = (Uint32)TRUE;
    }

    /*  Initialize CPU Interface sub-module */
    if(FALSE == cpuIntrf->initStatus)
    {
        /* Configure the priority of PPI and SGI interrupts (for primary GIC only) */
        /* Disable both SGI and PPI interrupts. Ability to disable SGI interrupts
           is implementation defined. But if it is RAZ/WI, still the below code
           will hold good. This will simplify the software implementation. */
        gicDistInst->GICD_ICENABLER[0] = CSL_ARM_GICD_DISABLE_INTR;
        for (intrNum = 0U; intrNum < 32U; intrNum += 4U)
        {
            gicDistInst->GICD_IPRIORITYR[intrNum/4U] = CSL_ARM_GICD_DISABLE_INTR;
        }

        /*
            Configure the priority mask to lowest value, so that all the priorities
            can be used. Based on the application need this can be modified apprioriately.
        */
        gicCpuInst->GICC_PMR = (Uint32)CSL_ARM_GIC_CPU_INTF_MIN_PRI_ALL;

        /* Non-Secure BPR */
        gicCpuInst->GICC_BPR = cpuIntrf->gicDist->nonSecureBinaryPoint;

        /* Store the register address for ISR handling */
#if defined(SOC_K2G) || defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2L) || defined(SOC_K2E)
        useAliasRegs = (Bool) TRUE;

#if defined(SOC_K2G)
        secureSystStatus = HW_RD_REG32_RAW(CSL_SEC_CTL_REGS + CSL_SEC_MGR_SYS_STATUS);
        secureSystStatus &= CSL_SEC_MGR_DEV_TYPE_MASK;

        /* check if the part is HS or not */
        if (secureSystStatus == (Uint32) CSL_SEC_MGR_DEV_TYPE_HS)
        {
          useAliasRegs = (Bool) FALSE;
        }
#endif /* SOC_K2G - override the address for secure part support on K2G */

        /* check if the part is HS or not */
       if (useAliasRegs == (Bool) TRUE)
       {
         cpuIntrf->iarRegAddress = (uintptr_t) &gicCpuInst->GICC_AIAR;
         cpuIntrf->eoiRegAddress = (uintptr_t) &gicCpuInst->GICC_AEOIR;
       }
       else
       {
         cpuIntrf->iarRegAddress = (uintptr_t) &gicCpuInst->GICC_IAR;
         cpuIntrf->eoiRegAddress = (uintptr_t) &gicCpuInst->GICC_EOIR;
       }
#else
        /*
            Security considerations need to be taken care by the application. If
            the security state of the CPU not matches with the active interrupt
            type special interrupt numbers 1022/1023 will be returned. It is the
            responsibility of the application to handle these interrupts.
        */
         cpuIntrf->iarRegAddress = (uintptr_t) &gicCpuInst->GICC_IAR;
         cpuIntrf->eoiRegAddress = (uintptr_t) &gicCpuInst->GICC_EOIR;
#endif

#if defined(SOC_K2G) || defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2L) || defined(SOC_K2E)
       /* Enable NS interrupts for KeyStone2 SoCs
        * 1011b = 0xB
        * Enable signaling of Group 0 interrupts
        * Enable signaling of Group 1 interrupts
        */
        ctrlConfigval = 0x3U;
#else
        /* Enable NS interrupts in Non-secure mode */
        ctrlConfigval = 0x1U;
#endif
        /* Enable CPU interface to signal the interrupt */
        gicCpuInst->GICC_CTLR = ctrlConfigval;

        cpuIntrf->initStatus = (Uint32)TRUE;
    }
}

static inline void CSL_armGicEnableIntr(const CSL_ArmGicCpuIntrf *cpuIntrf, Uint32 intrNum);
static inline void CSL_armGicEnableIntr
(
    const CSL_ArmGicCpuIntrf *cpuIntrf,
    Uint32 intrNum
)
{
    CSL_ArmgicDistributorRegs *gicDistInst = NULL;

    gicDistInst = (CSL_ArmgicDistributorRegs *)cpuIntrf->gicDist->distBasePtr;

    /*  Disable the interrupt */
    if((intrNum / 32U) < 16U)
    {
        gicDistInst->GICD_ISENABLER[intrNum/32U] = CSL_ARM_GICD_SET_CLR_MASK(intrNum);
    }
}


static inline void CSL_armGicDisableIntr(const CSL_ArmGicCpuIntrf *cpuIntrf, Uint32 intrNum);
static inline void CSL_armGicDisableIntr
        (
                const CSL_ArmGicCpuIntrf *cpuIntrf,
                Uint32 intrNum
        )
{
    CSL_ArmgicDistributorRegs *gicDistInst = NULL;

    gicDistInst = (CSL_ArmgicDistributorRegs *)cpuIntrf->gicDist->distBasePtr;

    /*  Disable the interrupt */
    if((intrNum / 32U) < 16U)
    {
        gicDistInst->GICD_ICENABLER[intrNum/32U] = CSL_ARM_GICD_SET_CLR_MASK(intrNum);
    }
}

#define HW_SYNC_BARRIER() asm volatile ("dsb")

static inline uint32_t HW_RD_REG32_RAW(uint32_t addr)
{
    uint32_t regVal = *(volatile uint32_t *) ((uintptr_t) addr);
    /* Donot call any functions after this. If required implement as macros */
    HW_SYNC_BARRIER();
    return (regVal);
}

static inline void HW_WR_REG32_RAW(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t *) ((uintptr_t) addr) = value;
    /* Donot call any functions after this. If required implement as macros */
    HW_SYNC_BARRIER();
    return;
}

//void OS_CPU_EOI () {
//    CSL_ArmGicCpuIntrf *cpuIntrf = &gCpuIntrf;
//    Uint32 intrAckVal   = (Uint32) HW_RD_REG32_RAW(cpuIntrf->iarRegAddress);
//    HW_WR_REG32_RAW(cpuIntrf->eoiRegAddress, intrAckVal);
//    timer_clear_irq();
//}

u32 gic_get_active_state(int intrNum) {
    CSL_ArmgicDistributorRegs *gicDistInst = NULL;
    gicDistInst = gCpuIntrf.gicDist->distBasePtr;
    if((intrNum / 32U) < 16U)
    {
        return gicDistInst->GICD_ISACTIVER[intrNum/32U] & CSL_ARM_GICD_SET_CLR_MASK(intrNum);
    }
    return 0;
}

u32 gic_get_pending_state(int intrNum) {
    CSL_ArmgicDistributorRegs *gicDistInst = NULL;
    gicDistInst = gCpuIntrf.gicDist->distBasePtr;
    if((intrNum / 32U) < 16U)
    {
        return gicDistInst->GICD_ISPENDR[intrNum/32U] & CSL_ARM_GICD_SET_CLR_MASK(intrNum);
    }
    return 0;
}

void gic_clear_pending_state(int intrNum) {
    CSL_ArmgicDistributorRegs *gicDistInst = NULL;
    gicDistInst = gCpuIntrf.gicDist->distBasePtr;
    if((intrNum / 32U) < 16U)
    {
        gicDistInst->GICD_ICPENDR[intrNum/32U] = CSL_ARM_GICD_SET_CLR_MASK(intrNum);
    }
}

//void gic_disable_irq(int num) {
//    u32 index = (num >> 0x05u) + 1u;
//    u32 mask = 1u << (num & 0x1fu);
//
//    HW_WR_REG32_RAW(SOC_INTC_MPU_DISTRIBUTOR_BASE +
//                            (/*ICER0*/ 0x180u + (4u * index)), mask);
//}


void CPU_GIC_Init (void) {
    gCpuIntrf.gicDist = &distrIntrf;
    gCpuIntrf.cpuIntfBasePtr = (void *)SOC_INTC_MPU_PHYS_CPU_IF_BASE;
    distrIntrf.distBasePtr = (void *)SOC_INTC_MPU_DISTRIBUTOR_BASE;
    gCpuIntrf.initStatus = (uint32_t)FALSE;
    gCpuIntrf.gicDist->initStatus = (uint32_t)FALSE;
    gCpuIntrf.pDefaultIntrHandlers = &CSL_armGicDefaultHandler;
    gCpuIntrf.pDefaultUserParameter = NULL;

    CSL_armGicInit(&gCpuIntrf);
    uart_puts("GIC init finished.\n");
    CSL_armGicEnableIntr(&gCpuIntrf, 70); // Timer2 ID70
    HW_WR_REG32_RAW(0x4a002a4c, 0); // crossbar setting: disable firewall error irq
}
