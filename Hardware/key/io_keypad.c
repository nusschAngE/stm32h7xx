
#include "stm32h7xx.h"

#include "io_keypad.h"



/********************PUBLIC FUNCTION **********************/
void ioKey_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           
    __HAL_RCC_GPIOC_CLK_ENABLE();          
    __HAL_RCC_GPIOH_CLK_ENABLE();          

    /* KEY_3 Wakeup */
    GPIO_Initure.Pin = GPIO_PIN_0;           
    GPIO_Initure.Mode = GPIO_MODE_INPUT;      
    GPIO_Initure.Pull = GPIO_PULLDOWN;       
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    /* KEY_2 */
    GPIO_Initure.Pin = GPIO_PIN_13;          
    GPIO_Initure.Mode = GPIO_MODE_INPUT;     
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    /* KEY_1, KEY_0 */
    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3; 
    GPIO_Initure.Mode = GPIO_MODE_INPUT;     
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

KEY_SCAN_VAL ioKey_GetKey(void)
{
    KEY_SCAN_VAL stat = 0x0000;

    /* wakeup key */
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
        stat |= (1 << GPIO_KEY3);
    else
        stat &= ~(1 << GPIO_KEY3);

    /* normal key */
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET)
        stat |= (1 << GPIO_KEY2);
    else
        stat &= ~(1 << GPIO_KEY2);

    if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_2) == GPIO_PIN_RESET)
        stat |= (1 << GPIO_KEY1);
    else 
        stat &= ~(1 << GPIO_KEY1);

    if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_3) == GPIO_PIN_RESET)
        stat |= (1 << GPIO_KEY0);
    else
        stat &= ~(1 << GPIO_KEY0);

    return stat;
}






