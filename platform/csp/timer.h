#ifndef UCOS2_TIMER_H
#define UCOS2_TIMER_H

#include "types.h"

void timer_init();

u32 timer_get_count();

u32 timer_get_status();

void timer_clear_irq();

#endif //UCOS2_TIMER_H
