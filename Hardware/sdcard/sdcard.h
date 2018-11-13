#ifndef _SDCARD_H
#define _SDCARD_H

#include "public.h"

//#define SDCARD_DMA_MODE     (1U)


enum
{
    SD_RESET                  = ((uint32_t)0x00000000U),  /*!< SD not yet initialized or disabled  */
    SD_READY                  = ((uint32_t)0x00000001U),  /*!< SD initialized and ready for use    */
    SD_TIMEOUT                = ((uint32_t)0x00000002U),  /*!< SD Timeout state                    */
    SD_BUSY                   = ((uint32_t)0x00000003U),  /*!< SD process ongoing                  */
    SD_PROGRAMMING            = ((uint32_t)0x00000004U),  /*!< SD Programming State                */
    SD_RECEIVING              = ((uint32_t)0x00000005U),  /*!< SD Receinving State                 */
    SD_TRANSFER               = ((uint32_t)0x00000006U),  /*!< SD Transfert State                  */
    SD_ERROR                  = ((uint32_t)0x0000000FU)   /*!< SD is in error state                */
};

/* for fatfs */
typedef struct
{
    uint32_t CardType;
    uint32_t CardVersion;

    uint32_t BlockNbr;
    uint32_t BlockSize;
}SD_BaseInfo;

extern SD_BaseInfo SDBaseInfo;
/*public function*/
uint8_t  SD_Init(void);
uint8_t  SD_GetCardState(void);
void SD_GetBaseInfo(SD_BaseInfo *info);
uint8_t SD_ReadMultipleBlocks(uint8_t *pBuff, uint32_t BlockAddr, uint32_t BlockNbr);
uint8_t SD_WriteMultipleBlocks(uint8_t *pData, uint32_t BlockAddr, uint32_t BlockNbr);


void sd_Test(void);




















#endif
