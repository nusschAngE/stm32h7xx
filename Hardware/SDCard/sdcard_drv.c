
#include "stm32h7xx.h"
#include "sdcard_drv.h"

SD_HandleTypeDef SD_Handler;
HAL_SD_CardInfoTypeDef SDCardInfo;

/****************** private functions **********************/

/* sd card controller clock & gpio initialize */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{    
	GPIO_InitTypeDef GPIO_Init;

    /* Peripheral clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();
  
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
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_Init);

    GPIO_Init.Pin = GPIO_PIN_2;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_Init);
}

/* sd card controller->SDMMC initialize */
static void SDIO_Init(void)
{

}

/* sd card initialize sequence */
static uint8_t sdcard_HardwareInit(void)
{
    /* include SDMMC Init & sd card init */
    if(HAL_SD_Init(&SD_Handler) == HAL_OK)
    {
        if(HAL_SD_GetCardInfo(&SD_Handler, &SDCardInfo) == HAL_OK)
        {
            return SD_RET_OK;
        }
    }

    return SD_RET_ERROR;
}

/****************** public functions **********************/

/*
*   SD Card initialize 
*/
uint8_t sdcard_Init(void)
{
    SDIO_Init();

    if(sdcard_HardwareInit() == SD_RET_OK)
    {
        return SD_RET_OK;
    }
    else
    {
        return SD_RET_ErrINIT;
    }
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
            ShortDelay(150);
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
            ShortDelay(150);
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



