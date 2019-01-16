
#include "stm32h7xx.h"
#include "delay.h"

#define DelayTIM		TIM8
#define DelayTIMCLK     (100000000)//APB2, max:100Mhz

#define DelayTIM_PSC_US		((DelayTIMCLK/1000/1000) - 1)
#define DelayTIM_ARR_1US	(1)

//#define DelayTIM_PSC_MS		((DelayTIMCLK/1000) - 1)
#define DelayTIM_ARR_1MS	(2000)

static __IO uint32_t HalDlyTicks = 0;
static TIM_HandleTypeDef DlyTIM_Handle;

uint8_t DelayModule_Init(void)
{
	TIM_ClockConfigTypeDef TimClock;

	__HAL_RCC_TIM8_CLK_ENABLE();
	DlyTIM_Handle.Instance = DelayTIM;
	DlyTIM_Handle.Init.Prescaler = DelayTIM_PSC_US;
	DlyTIM_Handle.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	DlyTIM_Handle.Init.Period = DelayTIM_ARR_1MS;
	DlyTIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	DlyTIM_Handle.Init.RepetitionCounter = 0x00;
	DlyTIM_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_Base_Init(&DlyTIM_Handle);
#if 0
	TimClock.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
	TimClock.ClockPolarity = TIM_ETRPOLARITY_NONINVERTED;
	TimClock.ClockPrescaler = TIM_ETRPRESCALER_DIV1;
	TimClock.ClockFilter = 0x00;
	HAL_TIM_ConfigClockSource(&DlyTIM_Handle, &TimClock);
#endif	

	__HAL_TIM_CLEAR_FLAG(&DlyTIM_Handle, TIM_FLAG_UPDATE);
	
    return (SYSTEM_ERROR_NONE);
}

void TimDelayUs(uint32_t us)
{
	volatile uint32_t count = us;

	/* internel delay */
	//if(us > 1)	count = us - 1;

	//__HAL_TIM_SET_PRESCALER(&DlyTIM_Handle, DelayTIM_PSC_US);
	__HAL_TIM_SET_AUTORELOAD(&DlyTIM_Handle, DelayTIM_ARR_1US);
	HAL_TIM_Base_Start(&DlyTIM_Handle);
	
	while(count--)
	{
		while(__HAL_TIM_GET_FLAG(&DlyTIM_Handle, TIM_FLAG_UPDATE) == RESET)
		{
		}
		__HAL_TIM_CLEAR_FLAG(&DlyTIM_Handle, TIM_FLAG_UPDATE);
	}

	HAL_TIM_Base_Stop(&DlyTIM_Handle);
}

void TimDelayMs(uint32_t ms)
{
	volatile uint32_t count = ms;

	//__HAL_TIM_SET_PRESCALER(&DlyTIM_Handle, DelayTIM_PSC_MS);
	__HAL_TIM_SET_AUTORELOAD(&DlyTIM_Handle, DelayTIM_ARR_1MS);
	HAL_TIM_Base_Start(&DlyTIM_Handle);
	
	while(count--)
	{
		while(__HAL_TIM_GET_FLAG(&DlyTIM_Handle, TIM_FLAG_UPDATE) == RESET)
		{
		}
		__HAL_TIM_CLEAR_FLAG(&DlyTIM_Handle, TIM_FLAG_UPDATE);
	}    

	HAL_TIM_Base_Stop(&DlyTIM_Handle);
}

void SysTickDelay(uint32_t ticks)
{

}

/*
*   ~1us in 400mhz
*   @para : us < 0xFFFF
*/
void DelayUs(uint16_t us)
{
    volatile uint32_t Count = 80 * us;

    while(Count--)
    {
    }
}

/*
*   ~1ms in 400mhz
*   @para : 
*/
void DelayMs(uint16_t ms)
{
    volatile uint64_t Count = 80000 * ms;

    while(Count--)
    {
    }
}

