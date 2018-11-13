
#include "stm32h7xx.h"

#include "ds18b20.h"
#include "delay.h"

/**
*   one DS18B20 only
*/

/* DQ */
#define TDQ_GPIO    GPIOB
#define TDQ_PIN     GPIO_PIN_12

#define DS18B20_DELAY(x)        TimDelayUs(x)

/* CMD */
#define DSCMD_SEARCH_ROM        (0xF0)
#define DSCMD_READ_ROM          (0x33)
#define DSCMD_MATCH_ROM         (0x55)
#define DSCMD_SKIP_ROM          (0xCC)
#define DSCMD_SEARCH_ALARM      (0xEC)

#define DSCMD_CONVERT           (0x44)
#define DSCMD_WRITE_SPAD        (0x4E)
#define DSCMD_READ_SPAD         (0xBE)
#define DSCMD_COPY_SPAD         (0x48)
#define DSCMD_RECALL_EEPROM     (0xB8)
#define DSCMD_READ_PWRMODE      (0xB4)

/* config register */
#define DS_CONFIG_REG           (0x1f)

/* in ms */
#define DS_CONVERT_TIME_9BIT    (750)//93.75ms
#define DS_CONVERT_TIME_10BIT   (750)//187.5ms
#define DS_CONVERT_TIME_11BIT   (750)//375ms
#define DS_CONVERT_TIME_12BIT   (750)//750ms

/*  STATIC
*/
static void TDQ_ConfigAsConvert(void);
static uint8_t ds_Reset(void);
static void ds_SendByte(uint8_t sVal);
static uint8_t ds_ReadByte(void);
#if 0
static uint8_t ds_CaculateCRC(uint16_t temp);
static uint8_t ds_ReadCRC(uint8_t *crc);
#endif

static uint16_t tempConvertTime = 0;
static char tempString[12];

/***************** PUBLIC FUNCTION ********************/
uint8_t DS18B20_Init(uint8_t th, uint8_t tl, uint8_t precise)
{
    uint8_t tmp = 0;
    
    if(ds_Reset())
    {
        ds_SendByte(DSCMD_SKIP_ROM);
        ds_SendByte(DSCMD_WRITE_SPAD);
        ds_SendByte(th);
        ds_SendByte(tl);
        /* 0x0**11111 */
        tmp = DS_CONFIG_REG | (precise << 5);
        ds_SendByte(tmp);
    
        switch (precise)
        {
            case DS_TEMP_12BIT:
                tempConvertTime = DS_CONVERT_TIME_12BIT;
                break;
            case DS_TEMP_11BIT:
                tempConvertTime = DS_CONVERT_TIME_11BIT;
                break;
            case DS_TEMP_10BIT:
                tempConvertTime = DS_CONVERT_TIME_10BIT;
                break;
            case DS_TEMP_9BIT:
            default:
                tempConvertTime = DS_CONVERT_TIME_9BIT;
                break;
        }
        return (1);
    }

    return (0);
}

uint8_t DS18B20_StartConvert(void)
{
    if(ds_Reset())
    {
        ds_SendByte(DSCMD_SKIP_ROM);
        ds_SendByte(DSCMD_CONVERT);
        TDQ_ConfigAsConvert();
        
        return (1);
    }

    return (0);
}

uint8_t DS18B20_ReadTemperature(uint16_t *tempVal)
{
    uint8_t msb, lsb;

    if(ds_Reset())
    {
        ds_SendByte(DSCMD_SKIP_ROM);
        ds_SendByte(DSCMD_READ_SPAD);
        lsb = ds_ReadByte();//lsb first
        msb = ds_ReadByte();

        *tempVal = ((uint16_t)msb << 8) | lsb;

        return (1);
    }
    
    return (0);
}

float DS18B20_CaculateTemperature(uint16_t tempVal)
{
    uint8_t pm = 0;
    float tmp = 0.0;

    pm = (tempVal & 0x8000) ? 1 : 0;//1 : minus, 0 : plus
    tmp = (tempVal & 0x7fff) * 0.625;

    return (pm) ? -tmp : tmp;
}

uint16_t DS18B20_GetConvertTime(void)
{
    return tempConvertTime;
}

char *DS18B20_Temp2String(float temp)
{
    pkgMemset(tempString, 0, sizeof(tempString));
    sprintf(tempString, "%.4f", temp);

    return tempString;
}

void DS18B20_Test(void)
{
    uint16_t value = 0;
    float temp;
    uint8_t ret;

    printf("DS18B20 test start...\r\n");
    ret = DS18B20_Init(0x2f, 0x2f, DS_TEMP_9BIT);
    if(ret == 0)
    {
        printf("DS18B20_Init error\r\n");
        while(1)
        {
        }
    }

    ret = DS18B20_StartConvert();
    if(ret == 0)
    {
        printf("DS18B20_StartConvert error\r\n");
        while(1)
        {
        }
    }

    TimDelayMs(tempConvertTime);
    ret = DS18B20_ReadTemperature(&value);
    if(ret == 0)
    {
        printf("DS18B20_ReadTemperature error\r\n");
        while(1)
        {
        }
    }

    temp = DS18B20_CaculateTemperature(value);
    printf("DS18B20 read value = 0x%04x\r\n", value);
    printf("DS18B20 read = %s\r\n", DS18B20_Temp2String(temp));
    printf("DS18B20 test completed!!\r\n");
}

/***************** PRIVATE FUNCTION *******************/
static inline void TDQ_ConfigAsOutput(void)
{
    GPIO_InitTypeDef GPIO_Init;

    GPIO_Init.Pin = TDQ_PIN;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(TDQ_GPIO, &GPIO_Init);
}

static inline void TDQ_ConfigAsInput(void)
{
    GPIO_InitTypeDef GPIO_Init;

    GPIO_Init.Pin = TDQ_PIN;
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(TDQ_GPIO, &GPIO_Init);
}

static inline uint8_t TDQ_Read(void)
{
    return HAL_GPIO_ReadPin(TDQ_GPIO, TDQ_PIN) ? 1 : 0;
}

static inline void TDQ_Set(uint8_t sta)
{
    HAL_GPIO_WritePin(TDQ_GPIO, TDQ_PIN, sta ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void TDQ_ConfigAsConvert(void)
{
	TDQ_Set(1);
    TDQ_ConfigAsOutput();
}

static uint8_t ds_Reset(void)
{
    uint16_t wait = 0xffff;

    TDQ_Set(1);
    TDQ_ConfigAsOutput();
    DS18B20_DELAY(5);
    /* reset */
    TDQ_Set(0);
    DS18B20_DELAY(550);
    TDQ_ConfigAsInput();
    DS18B20_DELAY(30);
    while(TDQ_Read())
    {
        DS18B20_DELAY(5);
        if(--wait == 0)
        {   
            return (0);
        }
    }
    DS18B20_DELAY(500);

    return (1);
}


static void ds_SendByte(uint8_t sVal)
{
    uint8_t i;
    
    TDQ_Set(1);
    TDQ_ConfigAsOutput();
    
    for(i = 0; i < 8; i++)
    {
    	TDQ_Set(1);
        DS18B20_DELAY(10);
        TDQ_Set(0);
        DS18B20_DELAY(13);
        
        TDQ_Set(sVal & 0x01);
        sVal >>= 1;
        
        DS18B20_DELAY(55);//must > 65us
    }
}

static uint8_t ds_ReadByte(void)
{
    uint8_t rVal = 0, i;
        
    for(i = 0; i < 8; i++)
    {
        TDQ_Set(1);
        TDQ_ConfigAsOutput();
        DS18B20_DELAY(10);
        TDQ_Set(0);
        DS18B20_DELAY(13);
        TDQ_ConfigAsInput();
        DS18B20_DELAY(25);

        rVal >>= 1;
        if(TDQ_Read())
            rVal |= 0x80;
        /* MSB First */
        
        DS18B20_DELAY(30);//must > 60us
    }

    return rVal;
}

#if 0
static uint8_t ds_CaculateCRC(uint16_t temp)
{
    
}

static uint8_t ds_ReadCRC(uint8_t *crc)
{

}
#endif

