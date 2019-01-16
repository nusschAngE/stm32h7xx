
#include "icon.h"
#include "icon_source.h"

#include "lcd.h"
#include "qspi_flash.h"

/* QSPI Read */
#define READ_ICON(cycle, addr, rsize)\
        QSPI_Flash_ReadWithCycle(cycle, addr, rsize)

/* total icons stored in flash */
#define NBR_OF_ICON     (sizeof(iconSources)/sizeof(ICON_SourceTypedef))

/*  */
static uint8_t cycleByte = 0;
static LCD_COLOR lcdData = 0;

static void lcdCycle(uint8_t data)
{
    /* receive one byte */
    ++cycleByte;
    /* get LCD_COLOR data */
    if(cycleByte == 1)
    {
        lcdData = data;
    }
    else//two bytes
    {
        cycleByte = 0;
        lcdData |= (uint16_t)data << 8;
        /* write to LCD */
        LCD->LCD_RAM = lcdData;
    }    
}

static void ramCycle(uint8_t data)
{
    
}

/* draw into LCD-GRAM */
void Icon_DrawScreen(uint16_t x, uint16_t y, ICON_SRC icon)
{
    ICON_SourceTypedef iconSrc;

    if(icon >= NBR_OF_ICON) {return ;}

    iconSrc = iconSources[icon];
    cycleByte = lcdData = 0;
    //LCD_SetWorkRegion(x, y, iconSrc.w, iconSrc.h);
    //LCD_StartDataSession();
    //READ_ICON(lcdCycle, iconSrc.addr, iconSrc.size);
    LCD_FillBlock(x, y, iconSrc.w, iconSrc.h, (uint16_t)iconSources[icon].addr);
}

/* draw into RAM-GRAM */
void Icon_DrawGRAM(uint16_t x, uint16_t y, ICON_SRC icon)
{
        
}
