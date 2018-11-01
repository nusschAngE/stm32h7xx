
#include "stm32h7xx.h"

#include "font.h"
#include "ff.h"
<<<<<<< HEAD
#include "qspi_flash.h"
=======
#include "SPIFlash_drv.h"
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

/*  Update Font 
**/
enum
{
    FONT_UPDATE_HZK12 = 0,
    FONT_UPDATE_HZK16,
    FONT_UPDATE_HZK24,
    FONT_UPDATE_HZK32,

    FONT_UPDATE_INFO,
    FONT_UPDATE_NUM
};

static const char *FontUpdatePath[] = 
{
    "/SYSTEM/FONT/HZK/GBK12.FON",	//GBK12.FON
    "/SYSTEM/FONT/HZK/GBK16.FON",	//GBK16.FON
    "/SYSTEM/FONT/HZK/GBK24.FON",	//GBK24.FON
    "/SYSTEM/FONT/HZK/GBK32.FON",	//GBK32.FON
    NULL,                           //INFO
};

/* font in flash */
<<<<<<< HEAD
FontInfo_Typedef FontInfo;
=======
_FontInfo FontInfo;
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

/*  update Font data, need file system.
*   save this data to SPI Flash
*
*   @para font : current update font
*   @para path : source file path
*   @para addr : save start address of SPI Flash
*   
*   @return :   0 -> ok, 
*               1 -> file error
*               2 -> font type error
*               3 -> write error
*/
static uint8_t font_ProcUpdate(uint8_t font, TCHAR *path)
{
    FIL FontFIL;

    FRESULT f_ret;//file system return status
    uint32_t br = 0;

    uint32_t beginAddr;
    uint32_t relSize, ofs = 0;
    uint8_t sf_ret;//flash return status
    uint8_t tempBuf[4096];//1 sector. too big, be careful!


    if(font >= FONT_UPDATE_NUM)
    {
        return 2;
    }
    else if(font == FONT_UPDATE_INFO)
    {
        //continue
    }
    else
    {
        f_ret = f_open(&FontFIL, (const TCHAR*)path, FA_READ);
        if(f_ret != FR_OK) 
            return 1;//file open error
    }
    
    switch (font)
    {
        case FONT_HZK12:
            FontInfo.hzk12Addr = FONT_PREADDR + sizeof(FontInfo);
            FontInfo.hzk12Size = FontFIL.obj.objsize;
            relSize = FontInfo.hzk12Size;
            beginAddr = FontInfo.hzk12Addr;
            break;
        case FONT_HZK16:
            FontInfo.hzk16Addr = FontInfo.hzk12Addr + FontInfo.hzk12Size;
            FontInfo.hzk16Size = FontFIL.obj.objsize;
            relSize = FontInfo.hzk16Size;
            beginAddr = FontInfo.hzk16Addr;
            break;
        case FONT_HZK24:
            FontInfo.hzk24Addr = FontInfo.hzk16Addr + FontInfo.hzk16Size;
            FontInfo.hzk24Size = FontFIL.obj.objsize;
            relSize = FontInfo.hzk24Size;
            beginAddr = FontInfo.hzk24Addr;
            break;
        case FONT_HZK32:
            FontInfo.hzk32Addr = FontInfo.hzk24Addr + FontInfo.hzk24Size;
            FontInfo.hzk32Size = FontFIL.obj.objsize;
            relSize = FontInfo.hzk32Size;
            beginAddr = FontInfo.hzk32Addr;
            break;
        case FONT_UPDATE_INFO: //font stored information
            relSize = sizeof(FontInfo);
            beginAddr = FONT_PREADDR;
            break;
    
    }

    while(relSize)
    {
        /* read file */
        f_ret = f_read(&FontFIL, (uint8_t *)tempBuf, sizeof(tempBuf), &br);
        if((f_ret != FR_OK) || (br == 0)) 
            break;

        /* write data to flash */
<<<<<<< HEAD
        sf_ret = spiFlash_WriteNoChk(tempBuf, beginAddr + ofs, br);
=======
        sf_ret = SPIFlash_WriteNoChk(tempBuf, beginAddr + ofs, br);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
        if(sf_ret != SPIFLASH_OK)
            break;

        ofs += br;
        relSize -= br;
    }

    f_close(&FontFIL);
    if(f_ret != FR_OK)
        return 1;
    
    if(sf_ret != SPIFLASH_OK)
        return 3;

    return 0;
}



/*
*   updata hzk font data.
*   mount disk before call this func.
*
*   @para drv : disk drive["0:" sd, "1:" usb, "2:" nand]
*/
void font_update(TCHAR *drv)
{
    FIL FontFIL;
    FRESULT f_ret;

    char FontSrc[64];
    uint8_t font;
    uint8_t sf_ret;
    uint16_t scrx = 20, scry = 20;

    for(font = 0; font < FONT_UPDATE_INFO; font++)
    {
<<<<<<< HEAD
        pkgMemset(FontSrc, 0, sizeof(FontSrc));
        pkgStrcpy((char *)FontSrc, (char *)drv);
        pkgStrcat((char *)FontSrc, (char *)FontUpdatePath[font]);
=======
        myMemset(FontSrc, 0, sizeof(FontSrc));
        myStrcpy((char *)FontSrc, (char *)drv);
        myStrcat((char *)FontSrc, (char *)FontUpdatePath[font]);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

        f_ret = f_open(&FontFIL, (const TCHAR*)FontSrc, FA_READ);
        if(f_ret != FR_OK)
        {
            //lcd debug out
            return ;
        }
    }

    //lcd debug out : flash erase
<<<<<<< HEAD
    sf_ret = spiFlash_EraseSectors(FONT_STARTSECTOR, FONT_PRESECTOR);
=======
    sf_ret = SPIFlash_EraseSectors(FONT_STARTSECTOR, FONT_PRESECTOR);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
    if(sf_ret != SPIFLASH_OK)
    {
        //lcd debug out : erase error
        return ;
    }

    //lcd debug out : update processing
    for(font = 0; font < FONT_UPDATE_NUM; font++)
    {
<<<<<<< HEAD
        pkgMemset(FontSrc, 0, sizeof(FontSrc));
        pkgStrcpy((char *)FontSrc, (char *)drv);
        pkgStrcat((char *)FontSrc, (char *)FontUpdatePath[font]);
=======
        myMemset(FontSrc, 0, sizeof(FontSrc));
        myStrcpy((char *)FontSrc, (char *)drv);
        myStrcat((char *)FontSrc, (char *)FontUpdatePath[font]);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56

        sf_ret = font_ProcUpdate(font, (TCHAR *)FontSrc);
        if(f_ret != 0)
        {
            //lcd debug out : update x error
            break ;
        }
    }

    if(f_ret == 0)
    {
        //lcd debug out : update completed
    }
    else
    {
        //lcd debug out : update error
    }
}












