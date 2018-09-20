
#ifndef _QSPI_FALSH_H
#define _QSPI_FALSH_H

#include "public.h"

#define QFLASH_SIZE             ((uint32_t)0x2000000) //32MByte
#define QFLASH_PAGESIZE         ((uint32_t)0x100)//256Byte
#define QFLASH_SECTORSIZE       ((uint32_t)0x1000)//4KBtye
#define QFLASH_BLOCKSIZE        ((uint32_t)0x10000)//64KByte

/*
*   SPI Flash usage
*/
// font stored
#define FONT_PRESIZE            ((uint32_t)0x700000)//7MByte , sector align
#define FONT_PREADDR            ((uint32_t)(0))//sector align
#define FONT_STARTSECTOR        ((uint32_t)(0))
#define FONT_PRESECTOR          ((uint32_t)(FONT_PRESIZE / QFLASH_SECTORSIZE))


/* SPI interface state */
typedef enum
{
    F_SPI_OK = 0,
    F_SPI_ERR
}F_SPI_State;

/* SPI Flash using SPI mode */
typedef enum
{
    F_SerialSPI     = 0,
    F_SerialDUALSPI,
    F_SerialQUADSPI,

    F_SerialNUM
}F_SerialType;

/* SPI Flash address mode */
typedef enum
{
    F_ADDR_3B = 0,
    F_ADDR_4B,
}F_AddressType;

/* SPI Flash state */
typedef enum
{   
    F_STA_INIT = 0,
    F_STA_OK,
    F_STA_READY,
    F_STA_BUSY,
    F_STA_ERROR
}SPIFlashStatus;


/* SPI Flash operate error code */
typedef enum
{
    F_OPR_OK = 0,
    F_OPR_ERR,
    F_OPR_ErrSPI,//serial mode
    F_OPR_ErrCMD,//send command error
    F_OPR_ErrWDIS,//write disable
    F_OPR_ErrWP,// write protect
    F_OPR_ErrAM,// address mode error 
    F_OPR_ErrTO,// timeout
    F_OPR_ErrSIZE, // target size error
    F_OPR_ErrADDR, //address error
    F_OPR_ErrRP, //page read error
    F_OPR_ErrPPROG, //page program error
    F_OPR_ErrREAD, // read error
    F_OPR_ErrWS, //sector write error
    F_OPR_ErrES  //sector erase error
}SPIFlashOperateError;

typedef struct
{
    uint8_t SPIMode;
    uint8_t AddressMode;
    uint8_t bWrite;//write enable?
    uint8_t pWrite;//write protect?
    uint8_t state;

    uint32_t id;
    uint32_t memorySize; //total bytes
    uint32_t pageSize;   //bytes per page
    uint32_t sectorSize; //bytes per sector
    uint32_t totalSector;
    uint32_t blockSize;  //bytes per block   
    uint32_t totalBlock;	
}SPIFlashDevice;

/* SPI Flash device struct */
extern SPIFlashDevice QFlash;

/* public function */
void qspiflash_init(void);
void qspiflash_DeviceInit(void);
uint8_t qspiflash_EraseSectors(uint32_t sectorAddr, uint32_t sectorNbr);
uint8_t qspiflash_read(uint8_t *pBuff, uint32_t Addr, uint16_t rSize);
/* SPI Flash write, not check */
uint8_t qspiflash_write(uint8_t *pBuff, uint32_t Addr, uint32_t wSize);
/* SPI Flash write with check*/
uint8_t qspiflash_WriteWithErase(uint8_t *pBuff, uint32_t Addr, uint32_t wSize);

void qspiflash_RWTest(void);




#endif

