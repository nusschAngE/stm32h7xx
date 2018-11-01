
#include "stm32h7xx.h"
#include "touch_drv.h"
#include "delay.h"


#define TOUCH_ADDRESS	(0x28)

/*** register ***/
#define GT_CTRL_REG     (0x8040)//control
#define GT_CFG_REG      (0x8047)//config table
#define GT_CHK_REG      (0x80ff)//check sum
#define GT_ID_REG       (0x8140)//product ID

#define GT_GSTID_REG 	0x814e   	//GT9147 touch status
#define GT_TP1_REG 		0x8150  	//first point
#define GT_TP2_REG 		0x8158		//secont point
#define GT_TP3_REG 		0x8160		//third point
#define GT_TP4_REG 		0x8168		//fourth point
#define GT_TP5_REG 		0x8170		//fifth point

/***  ***/
#define PRODUCT_ID      "9147" 

/***  ***/
static uint16_t ReadPointTBL[] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG}; 

/***************** PRIVATE FCUNTION *****************/

static uint8_t GT9147_WriteRegister(uint16_t reg, uint8_t *sVal, uint8_t NumBytes)
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
static void GT9147_HardwareReset(void)
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
	uSleep(50000);
	/* reset high, reset sequence done */
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);
	uSleep(10000);
	
	/* set INT pin input */
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOH,&GPIO_Init);
}

static uint8_t GT9147_ReadInterrupt(void)
{
	return (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_7) ? 1 : 0);
}

/*  GT9147 internal config
*/
const uint8_t GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
}; 

/*  mode : 0->not save, 1->save to flash
*/
static void GT9147_Send_Cfg(uint8_t mode)
{
	uint8_t buf[2];
	uint8_t i = 0;
	buf[0] = 0;
	buf[1] = mode;

	/* get checksum */
	for(i = 0; i < sizeof(GT9147_CFG_TBL); i++)
	{
	    buf[0] += GT9147_CFG_TBL[i];
	}   
    buf[0] = (~buf[0]) + 1;

	GT9147_WriteRegister(GT_CFG_REG, (uint8_t*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));//send config table
	GT9147_WriteRegister(GT_CHK_REG, buf, 2);//send checksum and config mode
} 

/***************** PUBLIC FCUNTION *****************/
uint8_t GT9147_Init(void)
{
	uint8_t tmp[5], ret = 0;

	T_IIC_Init();
	GT9147_HardwareReset();
	uSleep(100000);

    /* read product ID */
    GT9147_ReadRegister(GT_ID_REG, tmp, 4);
    if(myStrcmp((const char*)tmp, PRODUCT_ID) != 0)
    {
        printf("Touch's Product ID read error!\r\n");
        return (TP_STA_ERRIC);
    }
    /* software reset */
    tmp[0] = 0x02;
    GT9147_WriteRegister(GT_CTRL_REG, tmp, 1);
    /* update config table */
    GT9147_ReadRegister(GT_CFG_REG, tmp, 1);
    if(tmp[0] < 0x60)
    {//if current cfg version < 0x60, update and save
        GT9147_Send_Cfg(1);
    }
    /* init done */
    uSleep(50000);
    tmp[0] = 0x00;
    GT9147_WriteRegister(GT_CTRL_REG, tmp, 1);

    printf("GT9147 init done!\r\n");

    return (TP_STA_OK);
}

/*  mode : 0->co-ordinate scan, 1->interrupt 
*/
uint8_t GT9147_Scan(uint8_t mode)
{
    static uint8_t chkCnt = 0;

    uint8_t tmp[6];
    uint8_t i, tp_point = 0;
    bool buffReady = FALSE;

    chkCnt++;
    if((chkCnt%5 == 0) || (chkCnt < 5))
    {
        if(mode)
        {
            
        }
        else
        {
            GT9147_ReadRegister(GT_GSTID_REG, tmp, 1);
            if(tmp[0] & 0x80)//buffer ready
            {
                buffReady = TRUE;
                tp_point = tmp[0] & 0x0f;
                tmp[0] = 0;
                GT9147_WriteRegister(GT_GSTID_REG, tmp, 1);
            }
        }

    }

    /*  buffer ready 
    *   no way to check which point is released.
    *   check one point touch or not touch only.
    */
    if((buffReady) && (tp_point == 1))
    {
        //for(i = 0; i < 1; i++)
        {
            myMemset(tmp, 0, 6);
            GT9147_ReadRegister(ReadPointTBL[0], tmp, 4);
            tpDev.xPos[0] = ((uint16_t)tmp[1] << 8) + tmp[0];
            tpDev.yPos[0] = ((uint16_t)tmp[3] << 8) + tmp[2];
            tpDev.actPoint = 1;
            //printf("touch : %d - %d\r\n", tpDev.xPos[0], tpDev.yPos[0]);
        }
    }

    /* current release, */
    if((buffReady) && (tp_point == 0))
    {
        tpDev.xPos[0] = 0xffff;
        tpDev.yPos[0] = 0xffff;
        tpDev.actPoint = 0;
    }

    if(chkCnt > 250) chkCnt = 0;

    return tp_point;
}

