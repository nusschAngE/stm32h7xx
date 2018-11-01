
#include "stm32h7xx.h"
#include "delay.h"
#include "system.h"

<<<<<<< HEAD
#if (RTOS_uCOS_II)
=======
#if (RTOS_uCOS_II == 1U)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
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
<<<<<<< HEAD
void uSleep(uint32_t us)
{
    volatile uint64_t Count = (SystemCoreClock / 6 / 1000000) * us;
=======
void uSleep(uint32_t dly)
{
    volatile uint64_t Count = (dly * 80);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

    while(Count--)
    {
    }
}

<<<<<<< HEAD
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
=======
#if (RTOS_uCOS_II == 1U)
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
void OSTaskSleep(uint32_t ticks)
{
    if(OSRunning)
    {
        OSTimeDly(ticks);
    }
}
#endif

