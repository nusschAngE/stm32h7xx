
#ifndef _DELAY_H
#define _DELAY_H

#include "public.h"

#define USE_SYSTICK_DELAY
//#define USE_TIMx_DELAY


#if defined(USE_SYSTICK_DELAY)
extern volatile uint32_t delay_ticks;
#endif



extern void DelayModule_Init(void);
extern void delay_us(uint32_t us);
extern void delay_ms(uint32_t ms);
extern void ShortDelay(uint16_t ticks);



#endif

