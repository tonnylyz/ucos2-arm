#ifndef UCOS2_ARM_MMC_H
#define UCOS2_ARM_MMC_H

#include "types.h"

int mmc_init();

int mmc_read_sector(u32 sector, char *buf);

#endif //UCOS2_ARM_MMC_H
