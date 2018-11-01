
#ifndef _DELAY_H
#define _DELAY_H

#include "public.h"



extern volatile uint32_t delayTicksCnt;


extern void DelayModule_Init(void);
extern void delay_us(uint32_t us);
extern void delay_ms(uint32_t ms);
extern void uSleep(uint32_t us);
extern void mSleep(uint32_t ms);
extern void SysTickDelay(uint32_t ticks);

#if (RTOS_uCOS_II)
extern void OSTaskSleep(uint32_t ticks);
#endif

#endif

