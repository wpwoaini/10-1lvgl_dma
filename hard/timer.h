#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
extern volatile uint32_t tick_count;
void Timer_Init(void);

#endif
