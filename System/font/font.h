
#ifndef _FONT_H_
#define _FONT_H_

#include "public.h"
#include "ff.h"

/*
*   ASCII Font 
*/
typedef enum
{
    FONT_ASC1206 = 0,
    FONT_ASC1608,
    FONT_ASC2412,
    FONT_ASC3216,

    FONT_ASCII_NUM
}ASCFontType;

/*
*   HZK GBK Font
*/
typedef enum
{
    FONT_HZK12 = 0,
    FONT_HZK16,
    FONT_HZK24,
    FONT_HZK32,

    FONT_NUM //use for unicode<->gbk map
}HZKFontType;


typedef struct
{
    uint32_t hzk12;     //exsit state 0/1. if read '0xff', SPIFlash is erased.
    uint32_t hzk12Addr; //start address in SPIFlash
    uint32_t hzk12Size; //total GBK size
    uint32_t hzk16;
    uint32_t hzk16Addr;
    uint32_t hzk16Size;
    uint32_t hzk24;
    uint32_t hzk24Addr;
    uint32_t hzk24Size;
    uint32_t hzk32;
    uint32_t hzk32Addr;
    uint32_t hzk32Size;
}FontInfo_Typedef;


/* font in flash */
extern FontInfo_Typedef FontInfo;

extern void font_update(TCHAR *drv);
















#endif

