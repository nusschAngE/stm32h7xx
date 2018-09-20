#ifndef _SDIO_SDCARD_H
#define _SDIO_SDCARD_H

#include "stm32h7xx.h"
#include "public.h"
#include "delay.h"

typedef enum
{
    SD_RET_OK = 0,
    SD_RET_ErrINIT,
    SD_RET_ErrERASE,
    SD_RET_ErrREAD,
    SD_RET_ErrWRITE,
    SD_RET_ErrTIMEOUT,
    
    SD_RET_ERROR
}SDReturn;

typedef enum
{
    SD_STATE_RESET                  = ((uint32_t)0x00000000U),  /*!< SD not yet initialized or disabled  */
    SD_STATE_READY                  = ((uint32_t)0x00000001U),  /*!< SD initialized and ready for use    */
    SD_STATE_TIMEOUT                = ((uint32_t)0x00000002U),  /*!< SD Timeout state                    */
    SD_STATE_BUSY                   = ((uint32_t)0x00000003U),  /*!< SD process ongoing                  */
    SD_STATE_PROGRAMMING            = ((uint32_t)0x00000004U),  /*!< SD Programming State                */
    SD_STATE_RECEIVING              = ((uint32_t)0x00000005U),  /*!< SD Receinving State                 */
    SD_STATE_TRANSFER               = ((uint32_t)0x00000006U),  /*!< SD Transfert State                  */
    SD_STATE_ERROR                  = ((uint32_t)0x0000000FU)   /*!< SD is in error state                */
}SDState;

/*sdcard info*/
extern HAL_SD_CardInfoTypeDef SDCardInfo;

/*public function*/
uint8_t sdcard_init(void);
uint8_t sdcard_GetState(void);
uint8_t sdcard_EraseBlocks(uint32_t StartBlock, uint32_t EndBlock);
uint8_t sdcard_ReadMultipleBlocks(uint8_t *pBuff, uint32_t BlockAddr, uint32_t BlockNbr);
uint8_t sdcard_WriteMultipleBlocks(uint8_t *pData, uint32_t BlockAddr, uint32_t BlockNbr);























#endif
