
#include "lcd.h"
#include "lcd_gram.h"

#define LCD_GRAM_SIZE   ((uint32_t)DEFAULT_LCD_WIDTH * DEFAULT_LCD_HEIGHT)

#ifndef ExtSDRAM_ENABLE
#error "LCD GRAM in ExtSDRAM!!"
#endif //ExtSDRAM_ENABLE

static LCD_COLOR lcdGram[LCD_GRAM_SIZE] _EXTSDRAM_RW;
//static LCD_COLOR lcdGram[16];
static __IO bool IsLCDGramInUse = FALSE;

/******** PRIVATE FUNCTION ****************/
static void GramXsferCplt(void)
{
	//printf(".");
    IsLCDGramInUse = FALSE;
}

/******** PUBLIC FUNCTION **********/

void LCDGram_FillBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, LCD_COLOR color)
{
    uint16_t x_end = x+w-1, y_end = y+h-1, x0 = x, y0 = y;
    uint16_t totalSize = w*h;

    while(IsLCDGramInUse != FALSE) //GRAM In Xsfer
    {
    }
    /* Fill RAM */
    for(; totalSize>=0; totalSize--)
    {
        lcdGram[y0*lcdDev.width + x0] = color;
        x0++;
        if(x0 >= x_end)
        {
        	x0 = x;
        	y0++;
        	if(y0 >= y_end) {break;}
        }
    }
}

void LCDGram_FillPicture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, LCD_COLOR *pic)
{
    uint16_t x_end = x+w-1, y_end = y+h-1, x0 = x, y0 = y;
    uint16_t totalSize = w*h;

    while(IsLCDGramInUse != FALSE) //GRAM In Xsfer
    {
    }
    /* Fill RAM */
    for(; totalSize>=0; totalSize--)
    {
        lcdGram[y0*lcdDev.width + x0] = *pic++;
        x0++;
        if(x0 >= x_end)
        {
        	x0 = x;
        	y0++;
        	if(y0 >= y_end) {break;}
        }
    }     
}

uint8_t LCDGram_DisplayInit(void)
{   
    uint8_t ret = 0;
    uint32_t i = 0;
    
    for(i=0; i<LCD_GRAM_SIZE; i++)
        lcdGram[i] = COLOR_WHITE;

    IsLCDGramInUse = FALSE;
    ret = LCD_DMAChannelInit(GramXsferCplt);

    return (ret);
}

void LCDGram_FrameDisplay(void)
{
	while(IsLCDGramInUse != FALSE)
	{
	}
	
    IsLCDGramInUse = TRUE;
    LCD_SetWorkRegion(0, 0, lcdDev.width, lcdDev.height);
    LCD_StartDataSession();
    LCD_DMAXsferStart(lcdGram, LCD_GRAM_SIZE);
    //LCD_FlushData(lcdGram, LCD_GRAM_SIZE);
    //LCD_FillPicture(0, 0, lcdDev.width, lcdDev.height, lcdGram);
    //IsLCDGramInUse = FALSE;
}

