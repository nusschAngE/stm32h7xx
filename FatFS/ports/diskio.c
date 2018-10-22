/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "sdcard_drv.h"

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SDMMC       0
#define DEV_USB         1
#define DEV_NAND        2


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	uint8_t result;

	switch (pdrv) {


	case DEV_SDMMC :
		result = sdcard_GetState();
        if(result == SD_STATE_ERROR)
        {
            stat |= STA_NODISK;
        }
        else
        {
            stat = 0;
        }
		return stat;
	
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	uint8_t result;

	switch (pdrv) {

	case DEV_SDMMC :
		result = sdcard_Init();
		if(result != SD_RET_OK)
        {
            stat |= STA_NOINIT;
        }
        else
        {
            stat = 0;
        }
		return stat;

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	uint8_t result;

	switch (pdrv) {

	case DEV_SDMMC :
		result = sdcard_ReadMultipleBlocks(buff, (uint32_t)sector, count);
		if(result == SD_RET_OK)
		{
		    res = RES_OK;
        }
		else
		{
		    res = RES_ERROR;
        }
		return res;

	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {

	case DEV_SDMMC :
		result = sdcard_WriteMultipleBlocks((BYTE *)buff, (uint32_t)sector, count);
		if(result == SD_RET_OK)
		{
		    res = RES_OK;
		}
		else
		{
		    res = RES_ERROR;
		}
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	
	case DEV_SDMMC :
		switch (cmd)
		{
		    case CTRL_SYNC:
                res = RES_OK;
		        break;
		    //case CTRL_TRIM:
            //    res = RES_OK;
		    //    break;
		    case GET_SECTOR_SIZE:
                *(WORD *)buff = SDCardInfo.LogBlockSize;
                res = RES_OK;
		        break;
		    case GET_BLOCK_SIZE:
                *(DWORD *)buff = SDCardInfo.LogBlockSize;
                res = RES_OK;
		        break;
            case GET_SECTOR_COUNT:
                *(DWORD *)buff = SDCardInfo.LogBlockNbr;
                res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
		}
		return res;

	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	return 0;
}

