
#include "stm32h7xx.h"
#include "delay.h"

#if defined(USE_SYSTICK_DELAY)
volatile uint32_t delay_ticks = 0;

/*
*   config system tick first, 1KHz
*   called after SysTick_Config(1KHz)
*/
void delay_ModuleInit(void)
{
    delay_ticks = 0;
}

void delay_us(uint32_t us)
{
    uint32_t ms = us / 1000;
    uint32_t rel = us % 1000;

    if(rel > 600)
    {
        ms += 1;
    }
    else
    {
        rel *= 380;
        while(rel--);
    }

    delay_ticks = ms;
    while(delay_ticks)
    {
    }
}

void delay_ms(uint32_t ms)
{
    delay_ticks = ms;
    while(delay_ticks)
    {
    }
}

/*
*   ticks < 1000
*/
void ShortDelay(uint16_t ticks)
{
    uint32_t Count = ticks * 350;

    while(Count--)
    {
    }
}

#elif defined(USE_TIMx_DELAY)

#endif





