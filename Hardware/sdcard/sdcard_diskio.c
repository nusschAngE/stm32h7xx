
#include "sdcard.h"
#include "sdcard_diskio.h"


DSTATUS SD_DiskInitialize(void)
{
    DSTATUS res = RES_ERROR;
    uint8_t sdret = 0;

    sdret = SDCard_Init();
    if(sdret == SD_ERROR_NONE)
    {
        res = RES_OK;
    }

    return (res);
}

DSTATUS SD_DiskGetStatus(void)
{
    DSTATUS res = RES_ERROR;
    uint8_t sdret = 0;

    sdret = SDCard_GetCardState();
    if(sdret == SD_READY)
    {
        res = RES_OK;
    }
    else if(sdret == SD_ERROR)
    {
        res = RES_ERROR;
    }
    else if(sdret == SD_BUSY)
    {
        res = RES_NOTRDY;
    }

    return (res);
}

DRESULT SD_DiskRead(BYTE *buff, DWORD sector, UINT count)
{
    DSTATUS res = RES_ERROR;
    uint8_t sdret = 0;

    sdret = SDCard_ReadMultipleBlocks((uint8_t*)buff, (uint32_t)sector, (uint32_t)count);
    if(sdret == SD_ERROR_NONE)
    {
        res = RES_OK;
    }

    return (res);
}

DRESULT SD_DiskWrite(const BYTE *buff, DWORD sector, UINT count)
{
    DSTATUS res = RES_ERROR;
    uint8_t sdret = 0;

    sdret = SDCard_WriteMultipleBlocks((uint8_t*)buff, (uint32_t)sector, (uint32_t)count);
    if(sdret == SD_ERROR_NONE)
    {
        res = RES_OK;
    }

    return (res);
}

DRESULT SD_DiskIoctl(BYTE cmd, void *buff)
{
    DSTATUS res = RES_ERROR;

    switch (cmd)
    {
	    case CTRL_SYNC:
            res = RES_OK;
		    break;
	    //case CTRL_TRIM:
        //    res = RES_OK;
	    //    break;
	    case GET_SECTOR_SIZE:
            *(WORD *)buff = SDBaseInfo.BlockSize;
            res = RES_OK;
		    break;
	    case GET_BLOCK_SIZE:
            *(DWORD *)buff = SDBaseInfo.BlockSize;
            res = RES_OK;
		    break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SDBaseInfo.BlockNbr;
            res = RES_OK;
		    break;
	    default:
		    res = RES_PARERR;
		    break;
    }

    return (res);
}










