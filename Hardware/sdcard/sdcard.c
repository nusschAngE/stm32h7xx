
#include "stm32h7xx.h"
#include "sdcard.h"
#include "delay.h"

static SD_HandleTypeDef SD_Handler;

SD_BaseInfo SDBaseInfo;
#if (SDCARD_DMA_MODE)
static volatile uint8_t SDReadCompleted = 0;
static volatile uint8_t SDWriteCompleted = 0;
#endif
/****************** PUBLIC FUNCTION **********************/

/*
*   SD Card initialize 
*/
uint8_t SD_Init(void)
{
	uint8_t ret;

	ret = HAL_SD_Init(&SD_Handler);
    if(ret != HAL_OK)
    {
        printf("HAL_SD_Init() return %d\r\n", ret);
        return (0)
    }

    SD_GetBaseInfo(&SDBaseInfo);
    printf("sdcard_Init() done\r\n");

    return (1);
}

uint8_t SD_GetCardState(void)
{
    HAL_SD_CardStateTypedef stat;

    stat = HAL_SD_GetCardState(&SD_Handler);
    if((stat == HAL_SD_CARD_READY) || (stat == HAL_SD_CARD_STANDBY))
    {
        return (SD_READY);
    }
    else if((stat == HAL_SD_CARD_ERROR) || (stat == HAL_SD_CARD_DISCONNECTED))
    {
        return (SD_ERROR);
    }
    else
    {
        return (SD_BUSY);
    }
}

void SD_GetBaseInfo(SD_BaseInfo *info)
{
    info->CardType      = SD_Handler.SdCard.CardType;
    info->CardVersion   = SD_Handler.SdCard.CardVersion;
    info->BlockNbr      = SD_Handler.SdCard.LogBlockNbr;
    info->BlockSize     = SD_Handler.SdCard.LogBlockSize;
}


/* SD Card read blocks */
uint8_t SD_ReadMultipleBlocks(uint8_t *pBuff, uint32_t BlockAddr, uint32_t BlockNbr)
{
    uint8_t ret;

#if (SDCARD_DMA_MODE)
    ret = HAL_SD_ReadBlocks_DMA(&SD_Handler, pBuff, BlockAddr, BlockNbr);
#else
    ret = HAL_SD_ReadBlocks(&SD_Handler, pBuff, BlockAddr, BlockNbr, 0xffff);
#endif
    if(ret == HAL_OK)
    {
#if (SDCARD_DMA_MODE)
        while(SDWriteCompleted == 0)
        {}
        SDWriteCompleted = 0;
#endif
        while(SD_GetCardState() != SD_READY)
        {}
        return (1);
    }

    return (0);
}

/* SD Card write blocks */
uint8_t SD_WriteMultipleBlocks(uint8_t *pData, uint32_t BlockAddr, uint32_t BlockNbr)
{
    uint8_t ret;
#if (SDCARD_DMA_MODE)
    ret = HAL_SD_WriteBlocks_DMA(&SD_Handler, pData, BlockAddr, BlockNbr);
#else
    ret = HAL_SD_WriteBlocks(&SD_Handler, pData, BlockAddr, BlockNbr, 0xffff);
#endif
    if(ret == HAL_OK)
    {
#if (SDCARD_DMA_MODE)
        while(SDReadCompleted == 0)
        {}
        SDReadCompleted = 0;
#endif
        while(SD_GetCardState() != SD_READY)
        {}
        return (1);
    }

    return (0);
}

void SD_Test(void)
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
	sta = SD_WriteMultipleBlocks(tmp, 1, 1);
	printf("sdcard write return = %d\r\n", sta);

	
	for(i = 0; i < 512; i++)
	{
		tmp[i] = 0;
	}
	sta = SD_ReadMultipleBlocks(tmp, 1, 1);
	printf("sdcard read return = %d\r\n", sta);

	for(i = 0; i < 512; i++)
	{
		printf("%d, ", tmp[i]);
	}
	printf("\r\n");
}


/*******************/
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

#if (SDCARD_DMA_MODE)
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
#endif
}

#if (SDCARD_DMA_MODE)
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&SD_Handler);
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
    SDWriteCompleted = 1;
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
    SDReadCompleted = 1;
}

#endif

