
#include "stm32h7xx.h"
#include "touch.h"
#include "delay.h"

/****************************************/

#define T_IIC_DELAY(x)      uSleep(x)
#define T_IIC_DELAY_TICK    (50)

enum
{
    T_IIC_OK = 0,
    T_IIC_TIMEOUT,
    T_IIC_ERROR,
};

static void T_IIC_Init(void);
static void T_IIC_Start(void);
static void T_IIC_SendByte(uint8_t sVal);
static uint8_t T_IIC_WaitACK(void);
static uint8_t T_IIC_ReadByte(void);
static void T_IIC_SendACK(void);
static void T_IIC_SendNACK(void);
static void T_IIC_Stop(void);

/**************** PRIVATE FUNCTION*********************/
#if (TOUCH_DRIVER_IC_GT9147)

#define TOUCH_ADDRESS	(0x28)

/***  ***/
#define PRODUCT_ID      "9147" 

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
static uint16_t ReadPointTBL[] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG}; 

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



/*  STATIC 
*/
static uint8_t GT9147_WriteRegister(uint16_t reg, uint8_t *sVal, uint8_t NumBytes);
static uint8_t GT9147_ReadRegister(uint16_t reg, uint8_t *rVal, uint8_t NumBytes);
static void GT9147_Send_Cfg(uint8_t mode);
static void GT9147_HardwareReset(void);
static uint8_t GT9147_ReadInterrupt(void);
static uint8_t GT9147_Init(void);
static uint8_t GT9147_Scan(uint8_t mode);
#endif

/***************** PUBLIC FCUNTION *****************/

/* TP DEVICE */
TP_Device tpDev;

/* device init */
void TOUCH_Init(void)
{
#if (TOUCH_DRIVER_IC_GT9147)
    if(GT9147_Init() == TP_STA_OK)
        tpDev.scanFunc = GT9147_Scan;
    else
        printf("touch[GT9147] init error!\r\n");
#endif
}

/********************** PRIVATE FUNCTION **************************/

#if (TOUCH_DRIVER_IC_GT9147)
/*  GT9147
*/
static uint8_t GT9147_WriteRegister(uint16_t reg, uint8_t *sVal, uint8_t NumBytes)
{
    uint8_t tmp;

	T_IIC_Start();
	T_IIC_SendByte(TOUCH_ADDRESS);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
	/* reg msb */
	tmp = (uint8_t)((reg & 0xff00) >> 8);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
	/* reg lsb */
	tmp = (uint8_t)(reg & 0x00ff);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
	/* send data */
	while(NumBytes--)
	{
		T_IIC_SendByte(*sVal);
		if(T_IIC_WaitACK())	
		    return (T_IIC_TIMEOUT);
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
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
	/* reg msb */
	tmp = (uint8_t)((reg & 0xff00) >> 8);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
	/* reg lsb */
	tmp = (uint8_t)(reg & 0x00ff);
	T_IIC_SendByte(tmp);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT); 
	/* restart */
	T_IIC_Stop();
	T_IIC_Start();
	/* read sequence */
	T_IIC_SendByte(TOUCH_ADDRESS + 1);
	if(T_IIC_WaitACK())	
	    return (T_IIC_TIMEOUT);
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

static uint8_t GT9147_Init(void)
{
	uint8_t tmp[5], ret = 0;

	T_IIC_Init();
	GT9147_HardwareReset();
	uSleep(100000);

	pkgMemset(tmp, 0, 5);
    /* read product ID */
    GT9147_ReadRegister(GT_ID_REG, tmp, 4);
    //printf("id tmp = %s\r\n", tmp);
    if(pkgStrcmp((const char*)tmp, PRODUCT_ID) != 0)
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
static uint8_t GT9147_Scan(uint8_t mode)
{
    uint8_t tmp[6];
    uint8_t i, tp_point = 0;
    bool buffReady = FALSE;

    if(mode == TP_SCAN_TRIGGER)
    {
            
    }
    else if(mode == TP_SCAN_POLLING)
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

    /*  buffer ready 
    */
    if(buffReady)
    {
        for(i = 0; i < TP_MAX_POINT; i++)
        {
            pkgMemset(tmp, 0, 6);
            GT9147_ReadRegister(ReadPointTBL[0], tmp, 4);
            tpDev.xPos[i] = ((uint16_t)tmp[1] << 8) + tmp[0];
            tpDev.yPos[i] = ((uint16_t)tmp[3] << 8) + tmp[2];
            //printf("touch : %d - %d\r\n", tpDev.xPos[0], tpDev.yPos[0]);
        }
        tpDev.actPoint = tp_point;
    }

    /* current release, */
    if((buffReady) && (tp_point == 0))
    {
        tpDev.actPoint = 0;
    }

    return tp_point;
}
#endif

/*  IIC function 
*/
static inline void T_SDA_ConfigAsInput(void)
{
	GPIO_InitTypeDef GPIO_Init;

	GPIO_Init.Pin = GPIO_PIN_3;
    GPIO_Init.Mode = GPIO_MODE_INPUT; 
    GPIO_Init.Pull = GPIO_PULLUP;        
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
    HAL_GPIO_Init(GPIOI, &GPIO_Init); 
}

static inline void T_SDA_ConfigAsOutput(void)
{
	GPIO_InitTypeDef GPIO_Init;

	GPIO_Init.Pin = GPIO_PIN_3;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_Init.Pull = GPIO_PULLUP;        
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
    HAL_GPIO_Init(GPIOI, &GPIO_Init); 
}

static inline void T_SDA_Set(uint8_t sta)
{
	if(sta)
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_3, GPIO_PIN_RESET);
}

static inline uint8_t T_SDA_Read(void)
{
	return HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_3) ? 1 : 0;
}

static inline void T_SCL_Set(uint8_t sta)
{
	if(sta)
		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
}

static void T_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Init;
    __HAL_RCC_GPIOH_CLK_ENABLE();		
	__HAL_RCC_GPIOI_CLK_ENABLE();		

    /* SCL */
    GPIO_Init.Pin = GPIO_PIN_6;          
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_Init.Pull = GPIO_PULLUP;        
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  
    HAL_GPIO_Init(GPIOH, &GPIO_Init);    
	/* SDA */
    GPIO_Init.Pin = GPIO_PIN_3;         
    HAL_GPIO_Init(GPIOI, &GPIO_Init);    
}

/*  IIC START : when CLK is high, change SDA from high ot low
*/
static void T_IIC_Start(void)
{	
    T_SCL_Set(1);
    T_SDA_Set(1);
    T_SDA_ConfigAsOutput();
    T_IIC_DELAY(50);
    T_SDA_Set(0);
    T_IIC_DELAY(T_IIC_DELAY_TICK);
    T_SCL_Set(0);
}

static void T_IIC_SendByte(uint8_t sVal)
{
    uint8_t t;   
    
    T_SCL_Set(0);
	T_SDA_ConfigAsOutput(); 	    
	T_IIC_DELAY(T_IIC_DELAY_TICK);
	for(t = 0; t < 8; t++)
    {
		T_SDA_Set(sVal & 0x80);//MSB first
		sVal <<= 1; 		
		
		T_SCL_Set(1); 
		T_IIC_DELAY(T_IIC_DELAY_TICK);
		T_SCL_Set(0);	
		T_IIC_DELAY(T_IIC_DELAY_TICK);
    }
}

static uint8_t T_IIC_WaitACK(void)
{
    uint8_t cnt = 0;

	
    T_SDA_Set(1);
    T_SDA_ConfigAsInput();    
    T_SCL_Set(1);
    T_IIC_DELAY(T_IIC_DELAY_TICK);
    while(T_SDA_Read())
    {
        if(++cnt > 250)
        {
            T_IIC_Stop();
            return (1);
        }
        T_IIC_DELAY(T_IIC_DELAY_TICK);
    }

    return (0);
}

static uint8_t T_IIC_ReadByte(void)
{
	uint8_t i, receive = 0;

	T_SCL_Set(0); 
 	T_SDA_ConfigAsInput();
	T_IIC_DELAY(50);
	for(i = 0; i < 8; i++ )
	{ 
		T_SCL_Set(0); 	    	   
		T_IIC_DELAY(T_IIC_DELAY_TICK);
		T_SCL_Set(1);	 
		T_IIC_DELAY(T_IIC_DELAY_TICK);
		receive <<= 1;
		if(T_SDA_Read())
		{
		    receive |= 0x01;   
		}
	}	
	    
 	return (receive);
}

static void T_IIC_SendACK(void)
{
	T_SCL_Set(0);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
	T_SDA_Set(0);
	T_SDA_ConfigAsOutput();	
	T_IIC_DELAY(20);
	T_SCL_Set(1);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
}

static void T_IIC_SendNACK(void)
{
	T_SCL_Set(0);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
	T_SDA_Set(1);
	T_SDA_ConfigAsOutput();	
	T_IIC_DELAY(20);
	T_SCL_Set(1);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
}


/*  IIC STOP : when CLK is high, change SDA from low to high
*/
static void T_IIC_Stop(void)
{	
    T_SCL_Set(0);
    T_IIC_DELAY(2);
    T_SDA_Set(0);    
    T_SDA_ConfigAsOutput();
    T_IIC_DELAY(2);

    T_SCL_Set(1);
    T_IIC_DELAY(T_IIC_DELAY_TICK);
    T_SDA_Set(1);
    T_IIC_DELAY(50);
}


