
#include "stm32h7xx.h"
#include "led.h"

/********************* GLOBAL FUNCTION *****************************/
void led_Init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_Init.Pin = GPIO_PIN_1;//RED
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);

    GPIO_Init.Pin = GPIO_PIN_0;//GREEN
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);
}

void led_Toggle(uint8_t led)
{
    switch (led)
    {   
        case LED_GREEN: 
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            break;
        case LED_RED: 
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
            break;
        default:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
            break;
    }
}

void led_Onoff(uint8_t led, bool on)
{
    switch (led)
    {   
        case LED_GREEN: 
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;
        case LED_RED: 
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;
    }
}




