
#include "stm32h7xx.h"
#include "i2c_interface.h"
#include "delay.h"

/* iic gpio */
#define IIC_SCL_GPIO    GPIOH
#define IIC_SCL_PIN     GPIO_PIN_4

#define IIC_SDA_GPIO    GPIOH
#define IIC_SDA_PIN     GPIO_PIN_5

/*  */
#define IIC_SDA_DRV_IN()    {GPIOH->MODER &= ~(3 << (5 * 2)); GPIOH->MODER |= (0 << (5 * 2));}
#define IIC_SDA_DRV_OUT()   {GPIOH->MODER &= ~(3 << (5 * 2)); GPIOH->MODER |= (1 << (5 * 2));}

#define IIC_SCL_H()         HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET)
#define IIC_SCL_L()         HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET)

#define IIC_SDA_H()         HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET)
#define IIC_SDA_L()         HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET)

#define IIC_SDA_READ()      HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5)

/******************** private function *************************/
static void gpio_iic_start(void)
{
    IIC_SDA_DRV_OUT();
    IIC_SCL_H();
    IIC_SDA_H();
    uSleep(5);
    IIC_SCL_L();
    uSleep(5);
    IIC_SDA_L();
    uSleep(5);
}

static void gpio_iic_restart(void)
{
    
}

static void gpio_iic_stop(void)
{
    IIC_SDA_DRV_OUT();
    IIC_SCL_L();
    IIC_SDA_L();
    uSleep(5);
    IIC_SCL_H();
    IIC_SDA_H();
    uSleep(5);
}

static uint8_t gpio_iic_readACK(void)
{
    uint16_t retry = 255;
    uint8_t ret = 0;

    IIC_SCL_H();
    uSleep(4);
    IIC_SDA_H();
    IIC_SDA_DRV_IN();
    uSleep(4);

    while(IIC_SDA_READ())
    {
        if(--retry == 0)
        {
            ret = 1;
        }
    }

    if(ret == 0)
    {
        IIC_SCL_L();
        delay_us(3);
    }
    return ret;
}

static void gpio_iic_sendACK(void)
{
    IIC_SCL_L();
    uSleep(5);
    IIC_SDA_L();
    IIC_SDA_DRV_OUT();
    uSleep(5);
    IIC_SCL_H();
    uSleep(5);
    IIC_SCL_L();
    uSleep(3);
}

static void gpio_iic_sendNACK(void)
{
    IIC_SCL_L();
    uSleep(5);
    IIC_SDA_H();
    IIC_SDA_DRV_OUT();
    uSleep(5);
    IIC_SCL_H();
    uSleep(5);
    IIC_SCL_L();
    uSleep(3);    
}

static void gpio_iic_sendByte(uint8_t data)
{
    uint8_t i;

    IIC_SCL_L();
    uSleep(3);
    IIC_SDA_DRV_OUT();
    for(i = 0; i < 8; i++)
    {
        if(data & 0x80) 
            IIC_SDA_H();
        else
            IIC_SDA_L();

        data <<= 1;
        uSleep(2);
        IIC_SCL_H();
        uSleep(4);
        IIC_SCL_L();
        uSleep(3);
    }
}

static uint8_t gpio_iic_readByte(uint8_t ack)
{
    uint8_t i, temp = 0;

    IIC_SDA_DRV_IN();
    uSleep(2);
    for(i = 0; i < 8; i++)
    {
        IIC_SCL_L();
        uSleep(3);
        IIC_SCL_H();
        uSleep(2);
        temp <<= 1;
        if(IIC_SDA_READ())
            temp |= 0x01;
        else
            temp |= 0x00;

        uSleep(2);
    }

    if(ack)
        gpio_iic_sendACK();
    else
        gpio_iic_sendNACK();

    return temp;
}

/******************** public function *************************/
void i2c_init(void)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOH_CLK_ENABLE(); 
    
    GPIO_Init.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_Init.Pull = GPIO_PULLUP;         
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
    HAL_GPIO_Init(GPIOH, &GPIO_Init);

    IIC_SCL_H();
    IIC_SDA_H();
}

uint8_t i2c_SendOneByte(uint8_t Addr, uint8_t sVal)
{
    gpio_iic_start();
    gpio_iic_sendByte(Addr);
    if(gpio_iic_readACK())
    {
        //error, nack
        gpio_iic_stop();
        return 0;
    }
    gpio_iic_sendByte(sVal);
    if(gpio_iic_readACK())
    {
        //error, nack
        gpio_iic_stop();
        return 0;
    }
    gpio_iic_stop();
    return 1;
}

uint8_t i2c_SendMultipleBytes(uint8_t Addr, uint8_t* pVal, uint32_t size)
{
    return 0;
}

uint8_t i2c_ReadOneByte(uint8_t Addr, uint8_t* rVal)
{
    gpio_iic_start();
    gpio_iic_sendByte(Addr);
    if(gpio_iic_readACK())
    {
        //error, nack
        gpio_iic_stop();
        return 0;
    }
    *rVal = gpio_iic_readByte(0);//send NACK
    gpio_iic_stop();
    return 1;
}

uint8_t i2c_ReadMultipleBytes(uint8_t Addr, uint8_t* pVal, uint32_t size)
{
    return 0;
}



