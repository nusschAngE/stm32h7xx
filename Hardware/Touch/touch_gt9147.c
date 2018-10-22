
#include "stm32h7xx.h"
#include "touch_drv.h"
#include "delay.h"


#define TOUCH_ADDRESS	(0x28)

/***************** PRIVATE FCUNTION *****************/
static uint8_t GT9147_WriteRegister(uint16_t reg, uint8_t sVal)
{
    uint8_t tmp;

	T_IIC_Start();
	T_IIC_SendByte(TOUCH_ADDRESS);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg msb */
	tmp = (uint8_t)((reg & 0xff00) >> 8);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg lsb */
	tmp = (uint8_t)(reg & 0x00ff);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* send data */
	T_IIC_SendByte(sVal);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	
	T_IIC_Stop();
	
	return (T_IIC_OK);
}

static uint8_t GT9147_WriteMultipleRegisters(uint16_t reg, uint8_t *sVal, uint8_t NumBytes)
{
    uint8_t tmp;

	T_IIC_Start();
	T_IIC_SendByte(TOUCH_ADDRESS);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg msb */
	tmp = (uint8_t)((reg & 0xff00) >> 8);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg lsb */
	tmp = (uint8_t)(reg & 0x00ff);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* send data */
	while(NumBytes--)
	{
		T_IIC_SendByte(*sVal);
		if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
		/* next data */
		sVal++;
	}
	T_IIC_Stop();
	
	return (T_IIC_OK);
}


static uint8_t GT9147_ReadRegister(uint16_t reg, uint8_t *rVal, uint8_t NumBytes)
{
    uint8_t tmp;

	T_IIC_Start();
	T_IIC_SendByte(TOUCH_ADDRESS);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg msb */
	tmp = (uint8_t)((reg & 0xff00) >> 8);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* reg lsb */
	tmp = (uint8_t)(reg & 0x00ff);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT); 
	/* restart */
	T_IIC_Stop();
	T_IIC_Start();
	/* read sequence */
	T_IIC_SendByte(TOUCH_ADDRESS + 1);
	if(T_IIC_WaitACK())	return (T_IIC_TIMEOUT);
	/* read data */
	while(NumBytes--)
	{
		*rVal = T_IIC_ReadByte();
		rVal++;

		if(NumBytes)
			T_IIC_SendACK();
		else
			T_IIC_SendNACK();
	}
	T_IIC_Stop();

	return (T_IIC_OK);
}



/* GPIO Init */
static void GT9147_ResetAndInit(void)
{
	GPIO_InitTypeDef GPIO_Init;

	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	/* reset pin */
	GPIO_Init.Pin = GPIO_PIN_8;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOI,&GPIO_Init);

	/* INT detect pin */
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Pull = GPIO_PULLUP;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOH,&GPIO_Init);

	/* reset on */
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
	delay_ms(50);
	/* reset high, reset sequence done */
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);
	delay_ms(10);
	
	/* set INT pin input */
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOH,&GPIO_Init);
}

static void GT9147_HWReset(uint8_t on)
{
	
}

static uint8_t GT9147_ReadInt(void)
{
	return (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_7) ? 1 : 0);
}

/***************** PUBLIC FCUNTION *****************/
void touch_Init(void)
{
	uint8_t tmp[4];

	printf("GT9147 Init...\r\n");
	T_IIC_Init();
	GT9147_ResetAndInit();
	delay_ms(100);

	if(GT9147_ReadRegister(0x8140, tmp, 4) == T_IIC_OK)
	{
		printf("Touch's Product ID is '%s'\r\n", tmp);
	}
	else
	{
		printf("Touch driver init error\r\n");
	}

	GT9147_WriteRegister(0x804f, 0x44);
	tmp[0] = 0;
	GT9147_ReadRegister(0x804f, tmp, 1);
}




