
#include "stm32h7xx.h"
#include "delay.h"
#include "system.h"

#if (RTOS_uCOS_II == 1U)
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
void uSleep(uint32_t dly)
{
    volatile uint64_t Count = (dly * 80);

    while(Count--)
    {
    }
}

#if (RTOS_uCOS_II == 1U)
void OSTaskSleep(uint32_t ticks)
{
    if(OSRunning)
    {
        OSTimeDly(ticks);
    }
}
#endif

