
#include "stm32h7xx.h"
#include "led.h"

/* led device */
_ledDev ledRed;
_ledDev ledGreen;

void led_init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    /* device init */
    ledRed.GPIOx = GPIOB;
    ledRed.Pinx = GPIO_PIN_1;
    
    ledGreen.GPIOx = GPIOB;
    ledGreen.Pinx = GPIO_PIN_0;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_Init.Pin = ledRed.Pinx;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ledRed.GPIOx, &GPIO_Init);

    GPIO_Init.Pin = ledGreen.Pinx;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ledGreen.GPIOx, &GPIO_Init);

    HAL_GPIO_WritePin(ledRed.GPIOx, ledRed.Pinx, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ledGreen.GPIOx, ledGreen.Pinx, GPIO_PIN_SET);
}

void led_setOnOff(_ledDev *led, bool on)
{
    HAL_GPIO_WritePin(led->GPIOx, led->Pinx, (on ? GPIO_PIN_RESET : GPIO_PIN_SET));
}







