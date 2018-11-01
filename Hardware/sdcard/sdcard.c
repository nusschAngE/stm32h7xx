
#include "stm32h7xx.h"
#include "sdcard.h"
#include "delay.h"

static SD_HandleTypeDef SD_Handler;
static HAL_SD_CardInfoTypeDef SDCardInfo;

SD_BaseInfo SDBaseInfo;
/****************** PUBLIC FUNCTION **********************/

/* sd card controller clock & gpio initialize */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{    
	GPIO_InitTypeDef GPIO_Init;

    /* Peripheral clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
  
    /**SDMMC1 GPIO Configuration    
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD 
    */
    GPIO_Init.Pin = GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_10
    				| GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_Init);

    GPIO_Init.Pin = GPIO_PIN_2;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_Init);

	hsd->Instance = SDMMC1;
	hsd->Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	hsd->Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	hsd->Init.BusWide = SDMMC_BUS_WIDE_4B;
	hsd->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd->Init.ClockDiv = SDMMC_NSpeed_CLK_DIV;//input 200mhz, need 25mhz
}

/*
*   SD Card initialize 
*/
uint8_t sdcard_Init(void)
{
	uint8_t ret;

	ret = HAL_SD_Init(&SD_Handler);
	printf("SD Card HAL Init, ret = %d\r\n", ret);
    /* SDMMC Init & sd card init */
    if(ret == HAL_OK)
    {
        if(HAL_SD_GetCardInfo(&SD_Handler, &SDCardInfo) == HAL_OK)
        {
            sdcard_GetBaseInfo(&SDBaseInfo);
            return SD_RET_OK;
        }
    }

    return SD_RET_ERROR;
}

uint8_t sdcard_GetState(void)
{
    HAL_SD_CardStateTypedef stat;

    stat = HAL_SD_GetCardState(&SD_Handler);
    if(stat == HAL_SD_CARD_READY)
    {
        return (SD_STATE_READY);
    }
    else if(stat == HAL_SD_CARD_ERROR)
    {
        return (SD_STATE_ERROR);
    }
    else
    {
        return (SD_STATE_BUSY);
    }
}

void sdcard_GetBaseInfo(SD_BaseInfo *info)
{
    info->CardType      = SDCardInfo.CardType;
    info->CardVersion   = SDCardInfo.CardVersion;
    info->BlockNbr      = SDCardInfo.LogBlockNbr;
    info->BlockSize     = SDCardInfo.LogBlockSize;
}

uint8_t sdcard_EraseBlocks(uint32_t StartBlock, uint32_t EndBlock)
{
    if(HAL_SD_Erase(&SD_Handler, StartBlock, EndBlock) == HAL_OK)
        return SD_RET_OK;
    else
        return SD_RET_ErrERASE;
}

/* SD Card read blocks */
uint8_t sdcard_ReadMultipleBlocks(uint8_t *pBuff, uint32_t BlockAddr, uint32_t BlockNbr)
{
    uint8_t ret = SD_RET_ErrREAD;
    uint8_t check = 0xff;
    
    ret = HAL_SD_ReadBlocks(&SD_Handler, pBuff, BlockAddr, BlockNbr, 0xffff);
    if(ret == HAL_OK)
    {
        ret = SD_RET_OK;
        while(sdcard_GetState() != SD_STATE_READY)
        {
            uSleep(1500);
            if(--check == 0)
            {
                ret = SD_RET_ErrTIMEOUT;
            }
        }
    }
    else
    {
        ret = SD_RET_ErrREAD;
    }

    return (ret);
}

/* SD Card write blocks */
uint8_t sdcard_WriteMultipleBlocks(uint8_t *pData, uint32_t BlockAddr, uint32_t BlockNbr)
{
    uint8_t ret = SD_RET_ErrREAD;
    uint8_t check = 0xff;

    ret = HAL_SD_WriteBlocks(&SD_Handler, pData, BlockAddr, BlockNbr, 0xffff);
    if(ret == HAL_OK)
    {
        ret = SD_RET_OK;
        while(sdcard_GetState() != SD_STATE_READY)
        {
            uSleep(1500);
            if(--check == 0)
            {
                ret = SD_RET_ErrTIMEOUT;
            }
        }
    }
    else
    {
        ret = SD_RET_ErrREAD;
    }

    return (ret);
}

void sdcard_Test(void)
{
	HAL_SD_CardInfoTypeDef sdcard_Info;
	uint8_t sta;

	uint8_t tmp[512];
	uint16_t i;

	sta = (uint8_t)HAL_SD_GetCardInfo(&SD_Handler, &sdcard_Info);
	printf("get card info return = %d\r\n", sta);
	printf("sdcard->type = %d\r\n", sdcard_Info.CardType);
	printf("sdcard->version = %d\r\n", sdcard_Info.CardVersion);
	printf("sdcard->speed = %d\r\n", sdcard_Info.CardSpeed);
	printf("sdcard->class = %d\r\n", sdcard_Info.Class);

	for(i = 0; i < 512; i++)
	{
		tmp[i] = i;
	}
	sta = sdcard_WriteMultipleBlocks(tmp, 1, 1);
	printf("sdcard write return = %d\r\n", sta);

	
	for(i = 0; i < 512; i++)
	{
		tmp[i] = 0;
	}
	sta = sdcard_ReadMultipleBlocks(tmp, 1, 1);
	printf("sdcard read return = %d\r\n", sta);

	for(i = 0; i < 512; i++)
	{
		printf("%d, ", tmp[i]);
	}
	printf("\r\n");
}


