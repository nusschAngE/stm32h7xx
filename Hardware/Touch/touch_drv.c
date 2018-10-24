
#include "stm32h7xx.h"
#include "lcd_drv.h"
#include "touch_drv.h"
#include "delay.h"

#include "led_drv.h"

/****************************************/

#define T_IIC_DELAY(x)      uSleep(x)
#define T_IIC_DELAY_TICK    (50)

/***************** IIC function ********************/

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

void T_IIC_Init(void)
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
void T_IIC_Start(void)
{	
    T_SCL_Set(1);
    T_SDA_Set(1);
    T_SDA_ConfigAsOutput();
    T_IIC_DELAY(50);
    T_SDA_Set(0);
    T_IIC_DELAY(T_IIC_DELAY_TICK);
    T_SCL_Set(0);
}

void T_IIC_SendByte(uint8_t sVal)
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


uint8_t T_IIC_WaitACK(void)
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

uint8_t T_IIC_ReadByte(void)
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

void T_IIC_SendACK(void)
{
	T_SCL_Set(0);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
	T_SDA_Set(0);
	T_SDA_ConfigAsOutput();	
	T_IIC_DELAY(20);
	T_SCL_Set(1);
	T_IIC_DELAY(T_IIC_DELAY_TICK);
}

void T_IIC_SendNACK(void)
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
void T_IIC_Stop(void)
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


/*************** GLOBAL FUNCTION ****************/

/* TP DEVICE */
_tp_dev tpDev;
_tp_event tpEvent;

/* device init */
void touch_Init(void)
{
    if(lcdDev.id == 35510)
    {
        if(GT9147_Init() == TP_STA_OK)
            tpDev.scanFunc = GT9147_Scan;
        else
            printf("touch[GT9147] init error!\r\n");
    }
}

void touchScan_Task(void)
{
    static uint32_t cnt = 0;

    uint8_t testKey = 0;
    uint16_t NonActCount = 0;

    printf("touchScan_Task start...\r\n");
    while(1)
    {
        tpDev.scanFunc(0);
        uSleep(2000);

        if(tpDev.actPoint)
        {
            cnt++;

            if((cnt >= 5) && (cnt < 40))
            {
                if(tpEvent.key[0] == TP_KEY_NONE)
                {
                    tpEvent.key[0] = TP_KEY_DOWN;
                    tpEvent.xPos[0] = tpDev.xPos[0];
                    tpEvent.yPos[0] = tpDev.yPos[0];
                    testKey = TP_KEY_DOWN;//key down
                    printf("touch key down\r\n");
                }
            }
            else if(cnt > 450)
            {
                if(tpEvent.key[0] == TP_KEY_DOWN)
                {
                    tpEvent.key[0] = TP_KEY_HOLD;
                    tpEvent.xPos[0] = tpDev.xPos[0];
                    tpEvent.yPos[0] = tpDev.yPos[0];
                    testKey = TP_KEY_HOLD;//key hold
                    printf("touch key hold\r\n");
                }
            }
        }

        if(tpDev.actPoint == 0)
        {
            //process
            if((cnt >= 40) && (cnt < 450))
            {
                if(tpEvent.key[0] == TP_KEY_DOWN)
                {
                    tpEvent.key[0] = TP_KEY_UP_BEFORE_HOLD;
                    tpEvent.xPos[0] = tpDev.xPos[0];
                    tpEvent.yPos[0] = tpDev.yPos[0];
                    testKey = TP_KEY_UP_BEFORE_HOLD;//key down
                    printf("touch key up before hold\r\n");
                }
            }
            else if(cnt < 450)
            {
                if(tpEvent.key[0] == TP_KEY_HOLD)
                {
                    tpEvent.key[0] = TP_KEY_UP_AFTER_HOLD;
                    tpEvent.xPos[0] = tpDev.xPos[0];
                    tpEvent.yPos[0] = tpDev.yPos[0];
                    testKey = TP_KEY_UP_AFTER_HOLD;//key down
                    printf("touch key up after hold\r\n");
                }
            }
            
            cnt = 0;
        }
#if 1//test code
        {
            if(cnt == 0)
            {
                NonActCount++;
                if(NonActCount >= 0x2FF)
                {
                    NonActCount = 0;
                    tpEvent.key[0] = TP_KEY_NONE;
                    led_Onoff(LED_RED, FALSE);
                    led_Onoff(LED_GREEN, FALSE);
                }
            }
            if(testKey)
            {
                switch (testKey)
                {
                    case TP_KEY_DOWN:
                        led_Onoff(LED_RED, TRUE);
                        led_Onoff(LED_GREEN, FALSE);
                        break;
                    case TP_KEY_UP_BEFORE_HOLD:
                        led_Onoff(LED_RED, FALSE);
                        led_Onoff(LED_GREEN, TRUE);
                        break;
                    case TP_KEY_HOLD:
                        led_Onoff(LED_RED, TRUE);
                        led_Onoff(LED_GREEN, TRUE);
                        break;
                    case TP_KEY_UP_AFTER_HOLD:
                        led_Onoff(LED_RED, FALSE);
                        led_Onoff(LED_GREEN, FALSE);
                        break;
                }

                testKey = 0;
            }
        }
#endif
    }
}

