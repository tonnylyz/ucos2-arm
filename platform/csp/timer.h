#ifndef UCOS2_TIMER_H
#define UCOS2_TIMER_H

#include "types.h"

void timer_init();

u32 timer_get_count();

u32 timer_get_status();

#endif //UCOS2_TIMER_H
