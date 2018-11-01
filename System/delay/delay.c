
#include "stm32h7xx.h"
#include "delay.h"
#include "system.h"

#if (RTOS_uCOS_II)
#include <ucos_ii.h>
#endif

volatile uint32_t delayTicksCnt = 0;

void DelayModule_Init(void)
{
    
}

void delay_us(uint32_t us)
{

}

void delay_ms(uint32_t ms)
{
    
}

void SysTickDelay(uint32_t ticks)
{
    if(sysTickInitDone)
    {
        delayTicksCnt = ticks;
        while(delayTicksCnt)
        {
        }
    }
}

/*
*   ~1us
*/
void uSleep(uint32_t us)
{
    volatile uint64_t Count = (SystemCoreClock / 6 / 1000000) * us;

    while(Count--)
    {
    }
}

/*
*   ~1ms
*/
void mSleep(uint32_t ms)
{
    volatile uint64_t Count = (SystemCoreClock / 6 / 1000) * ms;

    while(Count--)
    {
    }
}

#if (RTOS_uCOS_II)
void OSTaskSleep(uint32_t ticks)
{
    if(OSRunning)
    {
        OSTimeDly(ticks);
    }
}
#endif

