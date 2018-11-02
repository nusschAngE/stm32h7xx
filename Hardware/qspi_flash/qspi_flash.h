
#ifndef _QSPI_FLASH_H
#define _QSPI_FLASH_H

#include "public.h"

#define SPIFLASH_MEMSIZE        ((uint32_t)0x2000000) //32MByte
#define SPIFLASH_PAGESIZE       ((uint32_t)0x100)//256Byte
#define SPIFLASH_SECTORSIZE     ((uint32_t)0x1000)//4KBtye
#define SPIFLASH_BLOCKSIZE      ((uint32_t)0x10000)//64KByte

#define SPIFLASH_PAGENBR        (SPIFLASH_MEMSIZE / SPIFLASH_PAGESIZE)
#define SPIFLASH_SECTORNBR      (SPIFLASH_MEMSIZE / SPIFLASH_SECTORSIZE)
#define SPIFLASH_BLOCKNBR       (SPIFLASH_MEMSIZE / SPIFLASH_BLOCKSIZE)

/*
*   SPI Flash memory usage
*/
// font stored
#define FONT_PRESIZE            ((uint32_t)0x700000)//7MByte , sector align
#define FONT_PREADDR            ((uint32_t)(0))//sector align
#define FONT_STARTSECTOR        ((uint32_t)(0))
#define FONT_PRESECTOR          ((uint32_t)(FONT_PRESIZE / SPIFLASH_SECTORSIZE))


/* SPI Flash return status */
enum
{
    SPIFLASH_OK             = 0,
    SPIFLASH_ERR_SERIAL     = 1,
    SPIFLASH_ERR_VALUE      = 2,
    SPIFLASH_ERR_ERASE      = 3,
    SPIFLASH_ERR_READ       = 4,
    SPIFLASH_ERR_WRITE      = 5,
    SPIFLASH_ERR_TIMEOUT    = 6,
    SPIFLASH_ERR_DEVICE		= 7,

    SPIFLASH_ERR,
};

/**************************** PUBLIC FUNCTION *******************************/

/* public function */
void SPIFlash_Init(void);
uint8_t SPIFlash_EraseSectors(uint32_t StartSector, uint32_t SectorNbr);
uint8_t SPIFlash_Read(uint8_t *pBuff, uint32_t Address, uint16_t rSize);
/* SPI Flash write, not check */
uint8_t SPIFlash_WriteNoChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize);
/* SPI Flash write with check*/
uint8_t SPIFlash_WriteWithChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize);

void SPIFlash_RWTest(void);


#endif

