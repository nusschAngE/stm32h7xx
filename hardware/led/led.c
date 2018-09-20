

#include "stm32h7xx.h"
#include "led.h"

LEDDev_Struct ledRed    = LED_DEVICE_FIELD(GPIOB, GPIO_PIN_1, FALSE, 0);
LEDDev_Struct ledGreen  = LED_DEVICE_FIELD(GPIOB, GPIO_PIN_0, FALSE, 0);
volatile uint8_t ledRedStatus = LED_INIT;
volatile uint8_t ledGreenStatus = LED_INIT;


void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    ledRedStatus = LED_INIT;
    ledGreenStatus = LED_INIT;

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

    ledRedStatus = LED_ON;
    ledRed.on = FALSE;
    ledRed.ticks = 0;
    
    ledGreenStatus = LED_BLINK;
    ledGreen.on = FALSE;
    ledGreen.ticks = 500;
}

/* led task call */
void LED_Onoff(LEDDev_Struct *led, uint8_t on)
{
    led->on = on;
    HAL_GPIO_WritePin(led->GPIOx, led->Pinx, (on ? GPIO_PIN_RESET : GPIO_PIN_SET));
}

#if 0
void led_blink(LEDDev_Struct *led, uint8_t on, uint32_t ticks)
{
    (void)ticks;
    HAL_GPIO_WritePin(led->GPIOx, led->Pinx, (on ? GPIO_PIN_RESET : GPIO_PIN_SET));
}
#endif








