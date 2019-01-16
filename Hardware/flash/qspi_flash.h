
#ifndef _QSPI_FLASH_H
#define _QSPI_FLASH_H

#include "public.h"

#define QSPIFLASH_MEMSIZE        ((uint32_t)0x2000000) //32MByte
#define QSPIFLASH_PAGESIZE       ((uint32_t)0x100)//256Byte
#define QSPIFLASH_SECTORSIZE     ((uint32_t)0x1000)//4KBtye
#define QSPIFLASH_BLOCKSIZE      ((uint32_t)0x10000)//64KByte

#define QSPIFLASH_PAGENBR        (QSPIFLASH_MEMSIZE / QSPIFLASH_PAGESIZE)
#define QSPIFLASH_SECTORNBR      (QSPIFLASH_MEMSIZE / QSPIFLASH_SECTORSIZE)
#define QSPIFLASH_BLOCKNBR       (QSPIFLASH_MEMSIZE / QSPIFLASH_BLOCKSIZE)

/* SPI Flash return status */
enum
{
    QSPIFLASH_ERROR_NONE         = 0,
    QSPIFLASH_ERROR_SERIAL       = 1,
    QSPIFLASH_ERROR_PARAM        = 2,
    QSPIFLASH_ERROR_ERASE        = 3,
    QSPIFLASH_ERROR_READ         = 4,
    QSPIFLASH_ERROR_WRITE        = 5,
    QSPIFLASH_ERROR_TIMEOUT      = 6,
    QSPIFLASH_ERROR_DEVICE	    = 7,
};

/* memory manage */

/* font
*/
#define HZKFONT_ADDR          	(25 * 1024 * 1024)//start at 25MB offset
#define HZKFONT_SIZE          	((6 * 1024 * 1024) + (512 * 1024))//total used 6.5MB
#define HZKFONT_START_SECTOR	(HZKFONT_ADDR / QSPIFLASH_SECTORSIZE)
#define HZKFONT_SECTORS       	(HZKFONT_SIZE / QSPIFLASH_SECTORSIZE)

/* IAP
*/
//IAP Bin Info[version, bin size, checkSum]
#define IAPINFO_ADDR            ((20 * 1024 * 1024) + (768 * 1024))
#define IAPINFO_SIZE            (256)
#define IAPINFO_START_SECTOR    (IAPINFO_ADDR / QSPIFLASH_SECTORSIZE)
#define IAPINFO_SECTORS         (IAPINFO_SIZE / QSPIFLASH_SECTORSIZE)
//this section is used for save the update bin
#define IAP0_ADDR               (IAPINFO_ADDR + IAPINFO_SIZE)
#define IAP0_SIZE               (2 * 1024 * 1024)
#define IAP0_START_SECTOR       (IAP0_ADDR / QSPIFLASH_SECTORSIZE)
#define IAP0_SECTORS       	    (IAP0_SIZE / QSPIFLASH_SECTORSIZE)
//this section is used for save the backup bin
#define IAP1_ADDR               (IAP1_ADDR + IAP0_SIZE)
#define IAP1_SIZE               (2 * 1024 * 1024)
#define IAP1_START_SECTOR       (IAP1_ADDR / QSPIFLASH_SECTORSIZE)
#define IAP1_SECTORS       	    (IAP1_SIZE / QSPIFLASH_SECTORSIZE)


/**************************** PUBLIC FUNCTION *******************************/

/* Flash read internal process */
typedef void(*cycle)(uint8_t data);//

/* public function */
uint8_t QSPI_Flash_Init(void);
uint8_t QSPI_Flash_EraseSectors(uint32_t StartSector, uint32_t SectorNbr);
uint8_t QSPI_Flash_Read(uint8_t *pBuff, uint32_t Address, uint16_t rSize);
uint8_t QSPI_Flash_ReadWithCycle(cycle cycleFunc, uint32_t Address, uint16_t rSize);
/* SPI Flash write, not check */
uint8_t QSPI_Flash_WriteNoChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize);
/* SPI Flash write with check*/
uint8_t QSPI_Flash_WriteWithChk(uint8_t *pBuff, uint32_t Address, uint32_t wSize);

#endif

