
#include "qspi_flash.h"
#include "lcd.h"

#include "font.h"
#include "ascii.h"
#include "text.h"
#include "my_malloc.h"
//#include "ff.h"

/*  ASCII Font
**/
typedef struct
{
    uint8_t  size;
    uint16_t binSize;
    uint16_t width;
    uint16_t height;
}ASCFont_Struct;

const ASCFont_Struct ASCFont[4] = 
{
    {FONT_SIZE_12,  12,  6, 12},
    {FONT_SIZE_16,  16,  8, 16},
    {FONT_SIZE_24,  36, 12, 24},
    {FONT_SIZE_32,  64, 16, 32}
};

/*  HZK Font
**/
typedef struct
{
    uint8_t  size;
    uint16_t binSize;
    uint16_t width;
    uint16_t height;
}HZKFont_Struct;

const HZKFont_Struct HZKFont[4] = 
{
    {FONT_SIZE_12,  24, 12, 12},
    {FONT_SIZE_16,  32, 16, 16},
    {FONT_SIZE_24,  72, 24, 24},
    {FONT_SIZE_32, 128, 32, 32}
};

/*  STATIC
*/
static uint16_t Fill_FontGRAM_ASCII(LCD_COLOR *ram, uint8_t ch, uint8_t size, LCD_COLOR fg, LCD_COLOR bg);
static uint16_t Fill_FontGRAM_GBK(LCD_COLOR *ram, uint16_t gbk, uint8_t size, LCD_COLOR fg, LCD_COLOR bg);
static void showASCII(uint16_t x, uint16_t y, uint8_t ch, Font_Typedef font);
static void showGBK(uint16_t x, uint16_t y, uint16_t gbk, Font_Typedef font);

/* font bmp data */
static uint8_t FontBIN[128];
/* font picture */
#define FONT_GRAM_SIZE  (32 * 32)
static LCD_COLOR FontGRAM[FONT_GRAM_SIZE];

/**************** PUBLIC FUNCTION *******************/

void ShowTextLineGbk(uint16_t x, uint16_t y, const uint8_t *pStr, Font_Typedef font)
{
    uint16_t i = 0;
    uint16_t x0 = x, y0 = y;
    uint16_t width, height;
    uint16_t bmpSize = 0;
    uint16_t code = 0;

    /* check work region */
    if(x >= lcdDev.width || y >= lcdDev.height)
    {
        return ;
    }

    for(i = 0; pStr[i] != 0; )
    {
        code = (uint16_t)pStr[i++];//lsb
        if(code < 0x80)//ascii
        {
            /* display */
            showASCII(x0, y0, (uint8_t)code, font);        
            /* posit */
            x0 += ASCFont[font.size].width;
        }
        else
        {
            code |= (uint16_t)pStr[i++] << 8;//msb
            /* display */
            showGBK(x0, y0, code, font);        
            /* posit */
            x0 += HZKFont[font.size].width;
        }
        /* check work region */
        if(x0 >= lcdDev.width)
        {
            break;
        }
    }
}

void ShowTextLineUnicode(uint16_t x, uint16_t y, const uint8_t *pStr, Font_Typedef font)
{
#if 0
    uint16_t x0 = x, y0 = y;
    uint32_t bWrite = 0, i = 0;
    uint16_t code = 0, uni = 0;

    if(x >= lcdDev.width || y >= lcdDev.height)
    {
        return ;
    }

    for(i = 0; pStr[i] || pStr[i+1]; )
    {
        uni = (uint16_t)pStr[i++];//lsb
        uni |= (uint16_t)pStr[i++] << 8;//msb
        /* unicode --> gbk */
        code = ff_uni2oem((DWORD)uni, FF_CODE_PAGE);
        if(code < 0x80)//ascii
        {
            /* display */
            showASCII(x0, y0, (uint8_t)code, font);        
            /* posit */
            x0 += ASCFont[font.size].width;
        }
        else
        {
            /* display */
            showGBK(x0, y0, code, font);        
            /* posit */
            x0 += HZKFont[font.size].width;
        }
        /* check work region */
        if(x0 >= lcdDev.width)
        {
            break;
        }
    }
#endif
}

void ShowTextLineAscii(uint16_t x, uint16_t y, const char *pStr, Font_Typedef font)
{
    uint16_t i = 0;
    uint16_t x0 = x, y0 = y;

    if(x >= lcdDev.width || y >= lcdDev.height)
    {
        return ;
    }

    for(i = 0; pStr[i] != '\0'; i++)
    {
        /* display */
        showASCII(x0, y0, pStr[i], font);        
        /* posit */
        x0 += ASCFont[font.size].width;
        /* check work region */
        if(x0 >= lcdDev.width)
        {
            break;
        }
    }
}

void ShowText(TextWorkRegion_Typedef region, Text_Typedef text, Font_Typedef font)
{
    
}

/******************* private function **********************/

/* 
*   @param size : font size
*/
static uint16_t Fill_FontGRAM_ASCII(LCD_COLOR *ram, uint8_t ch, uint8_t size, LCD_COLOR fg, LCD_COLOR bg)
{
    uint8_t ofs;
    uint16_t i,j;

    uint8_t x = 0, y = 0, temp = 0;
    uint8_t width, height;
    uint16_t FontBmpSize, FontBinSize;

    myMemset(FontBIN, 0, 128);

    /* unsupport font size */
    if(size >= FONT_SIZE_UN)
    {
        return (0);
    }
    
    width = ASCFont[size].width;
    height = ASCFont[size].height;
    FontBinSize = ASCFont[size].binSize;
    FontBmpSize = ASCFont[size].width * ASCFont[size].height;

	//printf("%d - %d - %d - %d - %d\r\n", size, width, height, FontBinSize, FontBmpSize);
    /* get 'ch' offset */
    ofs = ch - ' ';
    if(ofs >= ASCII_NUMS)   ofs = 0;
    /* get font bin */
    for(i = 0; i < FontBinSize; i++)
    {
        switch (size)
        {
            case FONT_SIZE_12: FontBIN[i] = asc2_1206[ofs][i]; break;
            case FONT_SIZE_16: FontBIN[i] = asc2_1608[ofs][i]; break;
            case FONT_SIZE_24: FontBIN[i] = asc2_2412[ofs][i]; break;
            case FONT_SIZE_32: FontBIN[i] = asc2_3216[ofs][i]; break;
        }
    }
    /* fill font bmp */
    for(i = 0; i < FontBinSize; i++)
    {
        temp = FontBIN[i];
        for(j = 0; j < 8; j++)
        {
            /* set dot */
            if(temp & 0x80) ram[x + (y * width)] = fg;
            else            ram[x + (y * width)] = bg;
            /* next dot */
            temp <<= 1;
            y++;
            if(y >= height)
            {
                y = 0;
                x++;
                break;
            }            
        }
    }
    
    return FontBmpSize;
}

/* 
*   @param size : font size
*/
static uint16_t Fill_FontGRAM_GBK(LCD_COLOR *ram, uint16_t gbk, uint8_t size, LCD_COLOR fg, LCD_COLOR bg)
{
    uint32_t ofs;
    uint16_t i,j;
    uint8_t hVal, lVal;
    uint8_t x = 0, y = 0, temp = 0;
    uint8_t width, height;
    uint16_t FontBmpSize, FontBinSize;

    myMemset(FontBIN, 0, 128);    

    /* unsupport font size */
    if(size >= FONT_SIZE_UN)
    {
        return (0);
    }

    width = HZKFont[size].width;
    height = HZKFont[size].height;
    FontBinSize = HZKFont[size].binSize;
    FontBmpSize = HZKFont[size].width * HZKFont[size].height;

    //hVal = (uint8_t)(gbk >> 8);
    //lVal = (uint8_t)(gbk & 0x00ff);
    hVal = (uint8_t)(gbk & 0x00ff);
    lVal = (uint8_t)(gbk >> 8);
    /* invalid code */
    if((hVal < 0x81) || (lVal < 0x40) || (hVal == 0xff) || (lVal== 0xff))
    {
        return (0);
    }
    /* special code process */
	if(lVal < 0x7f) lVal -= 0x40;
	else            lVal -= 0x41;
    /*  */
    hVal -= 0x81;
    //ofs = ((lVal - 0xA1)*94 + (hVal - 0xA1)) * FontBinSize;// font data offset
    ofs = (190 * hVal + lVal) * FontBinSize; //font data offset
    //printf("gbk = %d, %d - %d - %d, flash addr = 0x%08x\r\n", gbk, hVal, lVal, ofs, ofs + HzkFontInfo.hzk16Addr);
    /* get font bin */
    switch (size)
    {
        case FONT_SIZE_12:
            QSPI_Flash_Read(FontBIN, ofs + HzkFontInfo.hzk12Addr, FontBinSize);
            break;
        case FONT_SIZE_16:
            QSPI_Flash_Read(FontBIN, ofs + HzkFontInfo.hzk16Addr, FontBinSize);
            break;
        case FONT_SIZE_24:
            QSPI_Flash_Read(FontBIN, ofs + HzkFontInfo.hzk24Addr, FontBinSize);
            break;
        case FONT_SIZE_32:
            QSPI_Flash_Read(FontBIN, ofs + HzkFontInfo.hzk32Addr, FontBinSize);
            break;
    }
    
    /* fill font bmp */
    for(i = 0; i < FontBinSize; i++)
    {
        temp = FontBIN[i];
        for(j = 0; j < 8; j++)
        {
            /* set dot */
            if(temp & 0x80) ram[x + (y * width)] = fg;
            else            ram[x + (y * width)] = bg;
            /* next dot */
            temp <<= 1;
            y++;
            if(y >= height)
            {
                y = 0;
                x++;
                break;
            }            
        }
    }
                		
	return FontBmpSize;
}

static void showASCII(uint16_t x, uint16_t y, uint8_t ch, Font_Typedef font)
{
    uint16_t bmpSize = 0;
    uint16_t width, height;

    myMemset(FontGRAM, 0, FONT_GRAM_SIZE);
    /* get font bmp */
    Fill_FontGRAM_ASCII(FontGRAM, ch, font.size, font.fg, font.bg);
    /* get font size */
    width = ASCFont[font.size].width;
    height = ASCFont[font.size].height;
    /* display font bmp */
    LCD_FillPicture(x, y, width, height, FontGRAM);
}

static void showGBK(uint16_t x, uint16_t y, uint16_t gbk, Font_Typedef font)
{
    uint16_t bmpSize = 0;
    uint16_t width, height;

    myMemset(FontGRAM, 0, FONT_GRAM_SIZE);
    /* get font bmp */
    bmpSize = Fill_FontGRAM_GBK(FontGRAM, gbk, font.size, font.fg, font.bg);
    /* get font size */
    width = HZKFont[font.size].width;
    height = HZKFont[font.size].height;
    /* display font bmp */
    LCD_FillPicture(x, y, width, height, FontGRAM);
}


