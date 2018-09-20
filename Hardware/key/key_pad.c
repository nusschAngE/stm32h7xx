

#include "stm32h7xx.h"
#include "key_pad.h"


void KeyPad_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           
    __HAL_RCC_GPIOC_CLK_ENABLE();          
    __HAL_RCC_GPIOH_CLK_ENABLE();          

    /* KEY_0 Wakeup */
    GPIO_Initure.Pin = GPIO_PIN_0;           
    GPIO_Initure.Mode = GPIO_MODE_INPUT;      
    GPIO_Initure.Pull = GPIO_PULLDOWN;       
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    /* KEY_1 */
    GPIO_Initure.Pin = GPIO_PIN_13;          
    GPIO_Initure.Mode = GPIO_MODE_INPUT;     
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;   
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    /* KEY_2, KEY_3 */
    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3; 
    GPIO_Initure.Mode = GPIO_MODE_INPUT;     
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

uint8_t KeyPad_Read(uint8_t key)
{
    uint8_t stat = KEY_RELEASE;

    switch(key)
    {
        case KEY_0:
            {
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
                    stat = KEY_PRESS;
                else
                    stat = KEY_RELEASE;
            }
            break;
        case KEY_1:
            {
                if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET)
                    stat = KEY_PRESS;
                else
                    stat = KEY_RELEASE;
            }   
            break;
        case KEY_2:
            {
                if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_2) == GPIO_PIN_RESET)
                    stat = KEY_PRESS;
                else
                    stat = KEY_RELEASE;
            }
            break;
        case KEY_3:
            {
                if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_3) == GPIO_PIN_RESET)
                    stat = KEY_PRESS;
                else
                    stat = KEY_RELEASE;
            }
            break;  
        default: stat = KEY_RELEASE; break;
    }
    return stat;
}











