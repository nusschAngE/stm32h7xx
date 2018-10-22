
#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "public.h"

typedef struct
{
    uint32_t SYSCLK_Frequency;
    uint32_t PLL1CLK_Frequency;
    uint32_t PLL2CLK_Frequency;
    uint32_t PLL3CLK_Frequency;
}SystemClock_Struct;


extern SystemClock_Struct SystemClock;

extern void system_RCCConfig(void);
extern void system_PLL2Config(uint32_t m, uint32_t n, uint32_t p, uint32_t q, uint32_t r);
extern void system_PLL3Config(uint32_t m, uint32_t n, uint32_t p, uint32_t q, uint32_t r);
extern void system_SCBCacheConfig(void);
extern void system_MPUConfig(void);







#endif


