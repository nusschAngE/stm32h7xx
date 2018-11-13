
#include "stm32h7xx.h"

#include "sys_i2c.h"
#include "io_expand.h"
#include "delay.h"

/* i2c address */
#define PCF85xx_ADDR        (0x40)

/*  STATIC
*/
static uint8_t IOExStatus = 0x00;

static uint8_t PCF85xx_Init(void);
static uint8_t PCF85xx_SendByte(uint8_t sVal);
static uint8_t PCF85xx_ReadByte(uint8_t *rVal);

/**************** PUBLIC FUNCTION ****************/
uint8_t IOExpand_Init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_Init.Pin = GPIO_PIN_12;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);

	SYSI2C_Init();

    IOExStatus = 0xFF;//all IO set high
    if(PCF85xx_Init() == 1)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

/* set one pin */
void IOExpand_SetIOx(uint8_t IOx, uint8_t sta)
{
    if(sta) 
        IOExStatus |= IOx;
    else    
        IOExStatus &= ~ IOx;

    PCF85xx_SendByte(IOExStatus);
}

/* return pin level */
uint8_t IOExpand_GetIOx(uint8_t IOx)
{
    PCF85xx_ReadByte(&IOExStatus);

    return (IOExStatus & IOx) ? 1 : 0;
}

/* active low */
void IOExpand_IntCheck(void)
{
    static uint8_t IOExInt = 1;
    IOExInt = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);

    if(!IOExInt)
    {
        PCF85xx_ReadByte(&IOExStatus);
    }
}

/**************** PRIVATE FUNCTION ***********************/
static uint8_t PCF85xx_Init(void)
{
    if(PCF85xx_SendByte(IOExStatus) == IIC_OK)
        return (1);
    else
        return (0);
}

static uint8_t PCF85xx_SendByte(uint8_t sVal)
{
    SYSI2C_Start();
    SYSI2C_SendByte(PCF85xx_ADDR);
    if(SYSI2C_WaitACK()) 
        return (IIC_TIMEOUT);
        
    SYSI2C_SendByte(sVal);
    if(SYSI2C_WaitACK()) 
        return (IIC_TIMEOUT);
        
    SYSI2C_Stop();

    return (IIC_OK);
}

static uint8_t PCF85xx_ReadByte(uint8_t *rVal)
{
    SYSI2C_Start();
    SYSI2C_SendByte(PCF85xx_ADDR + 1);
    if(SYSI2C_WaitACK()) 
        return (IIC_TIMEOUT);
        
    *rVal = SYSI2C_ReadByte();
    SYSI2C_SendACK();
    SYSI2C_Stop();

    return (IIC_OK);
}



