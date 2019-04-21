#include "mmc.h"
#include "types.h"

#include <ti/csl/csl_mmcsd.h>
#include <ti/board/board.h>
#include <ti/csl/soc.h>
#include <ti/csl/soc/am572x/src/cslr_soc_mpu_baseaddress.h>
#include <ti/csl/soc/am572x/src/cslr_soc_aliases.h>
#include <snprintf.h>

#include <mmio.h>

/* Note:
 *  MMC1 -> removeable SD card, SDHC is addressing by block/sector (need NO initialization)
 *  MMC2 -> internal EMMC (NEED initialization)
 * */
static uint32_t baseAddress = KADDR(CSL_MPU_MMC1_REGS);
#define BUFFER_SIZE    (512)
#define BIT(x) (1 << x)
#define SD_CMD(x)   (x)
/* Command/Response flags for notifying some information to controller */
#define SD_CMDRSP_NONE      BIT(0)
#define SD_CMDRSP_STOP      BIT(1)
#define SD_CMDRSP_FS        BIT(2)
#define SD_CMDRSP_ABORT     BIT(3)
#define SD_CMDRSP_BUSY      BIT(4)
#define SD_CMDRSP_136BITS   BIT(5)
#define SD_CMDRSP_DATA      BIT(6)
#define SD_CMDRSP_READ      BIT(7)
#define SD_CMDRSP_WRITE     BIT(8)

/* Check RCA/status */
#define SD_RCA_ADDR(rca)             ((rca & 0xFFFF0000) >> 16)
#define SD_RCA_STAT(rca)             (rca & 0x0xFFFF)
#define MMCSD_IN_FREQ                    (96000000) /* 96MHz */
#define MMCSD_INIT_FREQ                  (400000)   /* 400kHz */

typedef struct _mmcsdCmd {
    unsigned int idx;
    unsigned int flags;
    unsigned int arg;
    signed char *data;
    unsigned int nblks;
    unsigned int rsp[4];
} mmcsdCmd;


uint32_t hsmmcsd_dataLen = 0;
uint32_t hsmmcsd_blockSize = 0;
volatile uint8_t *hsmmcsd_buffer = 0;
uint32_t hsmmcsd_cid[4], hsmmcsd_rca, hsmmcsd_csd[4];

/* MMCSD Host Controller Functions */
static int32_t mmcHostCtrlInit(void) {
    int32_t status = STW_SOK;
    uint32_t intrMask;
    /*Refer to the MMC Host and Bus configuration steps in TRM */
    /* controller Reset */
    status = HSMMCSDSoftReset(baseAddress);
    if (status != STW_SOK) {
        printf("\nHS MMC/SD Reset failed\n\r");
    }

    /* Lines Reset */
    status += HSMMCSDLinesReset(baseAddress, MMC_SYSCTL_SRA_MASK);
    /* Set supported voltage list */
    HSMMCSDSupportedVoltSet(baseAddress, MMC_CAPA_VS18_MASK |
                                         MMC_CAPA_VS30_MASK);
    HSMMCSDSystemConfig(baseAddress, MMC_SYSCONFIG_AUTOIDLE_MASK);
    /* Set the bus width to 1 bit*/
    HSMMCSDBusWidthSet(baseAddress, 0x1);

    /* Set the bus voltage */
    HSMMCSDBusVoltSet(baseAddress, (MMC_HCTL_SDVS_3V0 << MMC_HCTL_SDVS_SHIFT));
    /* Bus power on */
    status +=
            HSMMCSDBusPower(baseAddress,
                            (MMC_HCTL_SDBP_PWRON << MMC_HCTL_SDBP_SHIFT));
    /* Set the initialization frequency */
    status += HSMMCSDBusFreqSet(baseAddress, MMCSD_IN_FREQ, MMCSD_INIT_FREQ, 0);

    if (0U == HSMMCSDInitStreamSend(baseAddress)) {
        status = STW_EFAIL;
    }

    /* Enable interrupts */
    intrMask = (HS_MMCSD_INTR_CMDCOMP | HS_MMCSD_INTR_CMDTIMEOUT |
                HS_MMCSD_INTR_DATATIMEOUT | HS_MMCSD_INTR_TRNFCOMP);
    HSMMCSDIntrEnable(baseAddress, intrMask);

    if (status != STW_SOK) {
        printf("\nMMC Host Controller init failed\n\r");
    }

    return status;
}

static int32_t HSMMCSDCmdStatusGet(void) {
    int32_t status = STW_SOK;

    while (1) {
        status = HSMMCSDIntrStatusGet(baseAddress, 0xFFFFFFFF);
        if (status & HS_MMCSD_STAT_CMDCOMP) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_CMDCOMP);
            break;
        }
        if (status & HS_MMCSD_STAT_ERR) {
            if (status & HS_MMCSD_STAT_CMDTIMEOUT) {
                HSMMCSDIntrStatusClear(baseAddress,
                                       HS_MMCSD_STAT_CMDTIMEOUT);
                status = STW_ETIMEOUT;
            }
            status = STW_EFAIL;
            break;
        }
    }
    return status;
}

unsigned int HSMMCSDCmdSend(mmcsdCmd *c) {
    unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
    unsigned int dataPresent;
    unsigned int status = STW_SOK;
    unsigned int rspType;
    unsigned int cmdDir;
    unsigned int nblks;
    unsigned int cmd;

    if (c->flags & SD_CMDRSP_STOP) {
        cmdType = HS_MMCSD_CMD_TYPE_SUSPEND;
    } else if (c->flags & SD_CMDRSP_FS) {
        cmdType = HS_MMCSD_CMD_TYPE_FUNCSEL;
    } else if (c->flags & SD_CMDRSP_ABORT) {
        cmdType = HS_MMCSD_CMD_TYPE_ABORT;
    }

    cmdDir = (c->flags & SD_CMDRSP_READ) ? \
             HS_MMCSD_CMD_DIR_READ : HS_MMCSD_CMD_DIR_WRITE;

    dataPresent = (c->flags & SD_CMDRSP_DATA) ? 1 : 0;
    nblks = (dataPresent == 1) ? c->nblks : 0;

    if (c->flags & SD_CMDRSP_NONE) {
        rspType = HS_MMCSD_NO_RESPONSE;
    } else if (c->flags & SD_CMDRSP_136BITS) {
        rspType = HS_MMCSD_136BITS_RESPONSE;
    } else if (c->flags & SD_CMDRSP_BUSY) {
        rspType = HS_MMCSD_48BITS_BUSY_RESPONSE;
    } else {
        rspType = HS_MMCSD_48BITS_RESPONSE;
    }

    cmd = HS_MMCSD_CMD(c->idx, cmdType, rspType, cmdDir);

    if (dataPresent) {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_STAT_TRNFCOMP);

        HSMMCSDDataTimeoutSet(baseAddress, HS_MMCSD_DATA_TIMEOUT(27));
    }

    HSMMCSDCommandSend(baseAddress, cmd, c->arg, (void *) dataPresent,
                       nblks, ((uint32_t) 0));

    HSMMCSDCmdStatusGet();
    if (status == STW_SOK) {
        HSMMCSDResponseGet(baseAddress, (uint32_t *) c->rsp);
    }

    return status;
}

static void HSMMCSDXferSetup(unsigned char rwFlag,
                             void *ptr,
                             unsigned int nBlks) {
    HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_TRNFCOMP);

    if (rwFlag == 1) {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
        HSMMCSDIntrStatusEnable(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
        HSMMCSDIntrStatusDisable(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
    } else {
        HSMMCSDIntrStatusClear(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
        HSMMCSDIntrStatusEnable(baseAddress, HS_MMCSD_INTR_BUFWRRDY);
        HSMMCSDIntrStatusDisable(baseAddress, HS_MMCSD_INTR_BUFRDRDY);
    }

    HSMMCSDBlkLenSet(baseAddress, hsmmcsd_blockSize);
    hsmmcsd_dataLen = (nBlks * hsmmcsd_blockSize);
    hsmmcsd_buffer = (volatile unsigned char *) ptr;
}

static unsigned int HSMMCSDXferStatusGet() {
    volatile uint32_t status = 0;
    int32_t retVal = 0;
    volatile uint32_t i = 0, i_max = 0;
    volatile uint8_t *dst_bfr = hsmmcsd_buffer;
    volatile uint32_t temp;

    while (1) {
        status = HSMMCSDIntrStatusGet(baseAddress, 0xFFFFFFFF);

        if (status & HS_MMCSD_STAT_BUFRDRDY) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_BUFRDRDY);

            if (dst_bfr != NULL) {
                if (hsmmcsd_dataLen < hsmmcsd_blockSize) {
                    i_max = hsmmcsd_dataLen;
                } else {
                    i_max = hsmmcsd_blockSize;
                }
                /*Input data bfr will not be 4-byte aligned*/
                for (i = 0; i < i_max; i += 4) {
                    temp = HW_RD_REG32(baseAddress + MMC_DATA);
                    dst_bfr[i] = *((char *) &temp);
                    dst_bfr[i + 1] = *((char *) &temp + 1);
                    dst_bfr[i + 2] = *((char *) &temp + 2);
                    dst_bfr[i + 3] = *((char *) &temp + 3);
                }
                dst_bfr += i_max;
            }
        }
        if (status & HS_MMCSD_STAT_BUFWRRDY) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_BUFRDRDY);

            if (hsmmcsd_buffer != NULL) {
                for (i = 0; i < hsmmcsd_dataLen; i += 4) {
                    *((char *) &temp) = hsmmcsd_buffer[i];
                    *((char *) &temp + 1) = hsmmcsd_buffer[i + 1];
                    *((char *) &temp + 2) = hsmmcsd_buffer[i + 2];
                    *((char *) &temp + 3) = hsmmcsd_buffer[i + 3];
                    HW_WR_REG32(baseAddress + MMC_DATA, temp);
                }
            }
        }
        if (status & HS_MMCSD_STAT_DATATIMEOUT) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_DATATIMEOUT);
            retVal = STW_ETIMEOUT;
            break;
        }
        if (status & HS_MMCSD_STAT_TRNFCOMP) {
            HSMMCSDIntrStatusClear(baseAddress,
                                   HS_MMCSD_STAT_TRNFCOMP);
            retVal = STW_SOK;
            break;
        }
    }
    return retVal;
}

/* MMC Card Protocol Functions */
int32_t MMCSDCardReset(void) {
    int32_t status = STW_SOK;
    mmcsdCmd cmd;

    cmd.idx = SD_CMD(0);
    cmd.flags = SD_CMDRSP_NONE;
    cmd.arg = 0;

    status = HSMMCSDCmdSend(&cmd);

    return status;
}

int32_t MMCSDCardInit(void) {
    unsigned int status = 0;
    mmcsdCmd cmd;

    /* CMD0 - reset card */
    status = MMCSDCardReset();

    if (status != STW_SOK) {
        return STW_EFAIL;
    }
    /* TODO: Detect mmc or sd card and do the init */

    do {
        /* CMD1 - send oper voltage */
        cmd.idx = SD_CMD(1);
        cmd.flags = 0;
        cmd.arg = 0xFF8000 | (0x2 << 29);

        status = HSMMCSDCmdSend(&cmd);
    } while (!(*cmd.rsp & (0x1 << 31)));
    /* Send CMD2, to get the card identification register */
    cmd.idx = SD_CMD(2);
    cmd.flags = SD_CMDRSP_136BITS;
    cmd.arg = 0;

    status = HSMMCSDCmdSend(&cmd);

    hsmmcsd_cid[0] = cmd.rsp[0];
    hsmmcsd_cid[1] = cmd.rsp[1];
    hsmmcsd_cid[2] = cmd.rsp[2];
    hsmmcsd_cid[3] = cmd.rsp[3];

    if (status != STW_SOK) {
        return STW_EFAIL;
    }

    /* Send CMD3, to get the card relative address */
    cmd.idx = SD_CMD(3);
    cmd.flags = 0;
    cmd.arg = 0;

    status = HSMMCSDCmdSend(&cmd);

    hsmmcsd_rca = SD_RCA_ADDR(cmd.rsp[0]);

    if (status != STW_SOK) {
        return STW_EFAIL;
    }
    /* Send CMD9, to get the card specific data */
    cmd.idx = SD_CMD(9);
    cmd.flags = SD_CMDRSP_136BITS;
    cmd.arg = hsmmcsd_rca << 16;

    status = HSMMCSDCmdSend(&cmd);

    hsmmcsd_csd[0] = cmd.rsp[0];
    hsmmcsd_csd[1] = cmd.rsp[1];
    hsmmcsd_csd[2] = cmd.rsp[2];
    hsmmcsd_csd[3] = cmd.rsp[3];

    if (status != STW_SOK) {
        return STW_EFAIL;
    }

    // SD_CMD(13) to check card state
    HSMMCSDBlkLenSet(baseAddress, 512);
    cmd.idx = SD_CMD(16);
    cmd.flags = SD_CMDRSP_NONE;
    cmd.arg = 512;
    status = HSMMCSDCmdSend(&cmd);

    if (status != STW_SOK) {
        return STW_EFAIL;
    } else {
        /* TODO: Get the block size from csd */
        hsmmcsd_blockSize = 512;
    }

    /* Select the card */
    cmd.idx = SD_CMD(7);
    cmd.flags = SD_CMDRSP_BUSY;
    cmd.arg = hsmmcsd_rca << 16;

    status = HSMMCSDCmdSend(&cmd);

    if (status != STW_SOK) {
        return STW_EFAIL;
    }

    return STW_SOK;
}

#define panic(_) do { printf(_); while (1) { asm(" NOP"); }  } while(0)

void mmc_init() {
    /* Note:
     *  Do not mmc_init CSL_MPU_MMC1_REGS!
     *  MMC1 has been initialized in SPL.
     * */
    int32_t r;
    r = mmcHostCtrlInit();
    if (r != STW_SOK) {
        panic("mmcHostCtrlInit failed");
    }
    r = MMCSDCardInit();
    if (r != STW_SOK) {
        panic("MMCSDCardInit failed");
    }
}

int mmc_read_sector(u32 sector, char *buf) {
    int32_t  status = STW_SOK;
    mmcsdCmd cmd;
    HSMMCSDXferSetup(1, buf, 1);
    cmd.idx   = SD_CMD(17);
    cmd.flags = SD_CMDRSP_READ | SD_CMDRSP_DATA;
    cmd.arg   = sector;
    cmd.nblks = 1;
    status    = HSMMCSDCmdSend(&cmd);
    HSMMCSDXferStatusGet();
    return status;
}
