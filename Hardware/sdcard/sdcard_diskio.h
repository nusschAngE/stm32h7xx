

#ifndef _SDCARD_DISKIO_H
#define _SDCARD_DISKIO_H

#include "public.h"
#include "diskio.h"


DSTATUS SD_DiskInitialize(void);
DSTATUS SD_DiskGetStatus(void);
DRESULT SD_DiskRead(BYTE *buff, DWORD sector, UINT count);
DRESULT SD_DiskWrite(const BYTE *buff, DWORD sector, UINT count);
DRESULT SD_DiskIoctl(BYTE cmd, void *buff);





#endif


