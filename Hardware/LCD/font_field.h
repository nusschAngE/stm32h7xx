
#ifndef _FONT_FIELD_H
#define _FONT_FIELD_H

#include "font.h"
#include "ascii.h"
#include "SPIFlash_drv.h"

/*  ASCII Font
**/
typedef struct
{
    uint8_t  font;
    uint16_t bmpSize;
    uint16_t width;
    uint16_t height;
}ASCFont_Struct;

const ASCFont_Struct ASCFont[4] = 
{
    {FONT_ASC1206,  12,  6, 12},
    {FONT_ASC1608,  16,  8, 16},
    {FONT_ASC2412,  36, 12, 24},
    {FONT_ASC3216, 128, 32, 32}
};

/*  HZK Font
**/
typedef struct
{
    uint8_t  font;
    uint16_t bmpSize;
    uint16_t width;
    uint16_t height;
}HZKFont_Struct;

const HZKFont_Struct HZKFont[4] = 
{
    {FONT_HZK12,  24, 12, 12},
    {FONT_HZK16,  32, 16, 16},
    {FONT_HZK24,  72, 24, 24},
    {FONT_HZK32, 128, 32, 32}
};

/******************* private function **********************/

/*  font display
*/
static uint16_t Field_FontGRAM_ASCII(LCD_COLOR *ram, uint16_t size, uint8_t ch, uint8_t font, LCD_COLOR fr, LCD_COLOR bg)
{
    uint8_t ofs;
    uint16_t i,j,bmpSize =  0;

    uint8_t x = 0, y = 0, temp = 0;
    uint16_t bitmapSize = 0;

    if(font < FONT_ASCII_NUM)
    {
        bmpSize = ASCFont[font].bmpSize;
        bitmapSize = ASCFont[font].width * ASCFont[font].height;
    }
    else
    {
        bmpSize = 0;
        bitmapSize = 0;
    }

    if((bitmapSize > size) || (bitmapSize == 0)) 
        return 0;//error size

    ofs = ch - ' ';
    if(ofs >= ASCII_NUMS)
        ofs = 0;
    
    /* field data */
    for(i = 0; i < bmpSize; i++)
    {
        switch(font)
        {
            case FONT_ASC1206: temp = asc2_1206[ofs][i]; break;
            case FONT_ASC1608: temp = asc2_1608[ofs][i]; break;
            case FONT_ASC2412: temp = asc2_2412[ofs][i]; break;
            case FONT_ASC3216: temp = asc2_3216[ofs][i]; break;
            default : temp = 0; break;
        } 

        for(j = 0; j < 8; j++)
        {
            if(temp & 0x80) 
                ram[x + (y * ASCFont[font].width)] = fr;
            else 
                ram[x + (y * ASCFont[font].width)] = bg;

            temp <<= 1;
            y++;
            if(y >= ASCFont[font].height)
            {
                y = 0;
                x++;
                break;
            }            
        }
    }
    return bitmapSize;
}

static uint16_t Field_FontGRAM_HZK(LCD_COLOR *ram, uint16_t size, uint16_t gbk, uint8_t font, LCD_COLOR fr, LCD_COLOR bg)
{
    uint16_t i,j,bmpSize =  0;

    uint8_t x = 0, y = 0, temp = 0;
    uint16_t bitmapSize = 0;

    uint8_t FontData[128];
    uint32_t ofs = 0;
    uint8_t hVal, lVal;

    if(font < FONT_ASCII_NUM)
    {
        bmpSize = HZKFont[font].bmpSize;
        bitmapSize = HZKFont[font].width * HZKFont[font].height;
    }
    else
    {
        bmpSize = 0;
        bitmapSize = 0;
    }

    if((bitmapSize > size) || (bitmapSize == 0)) 
        return 0;//error size

    hVal = (uint8_t)(gbk >> 8);
    lVal = (uint8_t)(gbk & 0x00ff);
    /* invalid code */
    if((hVal < 0x81) || (lVal < 0x40) || (hVal == 0xff) || (lVal== 0xff))
	{   		    
	    for(i = 0; i < bmpSize; i++)
	        FontData[i] = 0x00;
	}  
	/* special code process */
	if(lVal < 0x7f)
	    lVal -= 0x40;
	else 
	    lVal -= 0x41;
	    
	hVal -= 0x81;   

    //offset = (bitCode - 0xA1)*94 + (areaCode - 0xA1)// charactor offset
	ofs = (190 * hVal + lVal) * bmpSize; //font data offset

    switch (font)
    {
        case FONT_HZK12:
            SPIFlash_Read(FontData, ofs + FontInfo.hzk12Addr, bmpSize);
            break;
        case FONT_HZK16:
            SPIFlash_Read(FontData, ofs + FontInfo.hzk16Addr, bmpSize);
            break;
        case FONT_HZK24:
            SPIFlash_Read(FontData, ofs + FontInfo.hzk24Addr, bmpSize);
            break;
        case FONT_HZK32:
            SPIFlash_Read(FontData, ofs + FontInfo.hzk32Addr, bmpSize);
            break;
        default : return 0;
    }

    for(i = 0; i < bmpSize; i++)
    {
        temp = FontData[i];
        for(j = 0; j < 8; j++)
        {
            if(temp & 0x80) 
                ram[x + (y * HZKFont[font].width)] = fr;
            else 
                ram[x + (y * HZKFont[font].width)] = bg;

            temp <<= 1;
            y++;
            if(y >= HZKFont[font].height)
            {
                y = 0;
                x++;
                break;
            }            
        }
    }
		
	return bitmapSize;
}











#endif


