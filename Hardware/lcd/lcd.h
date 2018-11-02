
#ifndef _LCD_H
#define _LCD_H

#include "public.h"
#include "font.h"

//LCD display
#define LCD_COLOR_FRAME         2U/*565*/
#if (LCD_COLOR_FRAME == 1U)
    typedef unsigned char  LCD_COLOR;
#elif (LCD_COLOR_FRAME == 2U)
    typedef unsigned short  LCD_COLOR;
#else
    typedef unsigned int  LCD_COLOR;
#endif

/* LCD size indicate in LCD's datasheet */
#define DEFAULT_LCD_WIDTH       ((uint16_t)480)
#define DEFAULT_LCD_HEIGHT      ((uint16_t)800)

enum
{
    LCD_OK = 0,
    LCD_BC, //cross the border
    LCD_ERR
};

enum
{
    LCD_FLUSH_COLOR = 0,
    LCD_FLUSH_PBUFF,
};

typedef struct
{
    uint32_t id;
    uint8_t  dir;
    uint16_t width;
    uint16_t height;
}LCD_Device;

/* LCD device struct */
extern LCD_Device lcdDev;


/** public func **/
void LCD_Init(void);
void LCD_SetOnOff(uint8_t on);
void LCD_SetBacklightOnOff(uint8_t on);
void LCD_SetScanDirection(uint8_t dir);
uint8_t LCD_SetWorkRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
uint8_t LCD_FlushData(LCD_COLOR *pData, uint32_t size, uint8_t type);
void LCD_Clear(LCD_COLOR color);
uint8_t LCD_FieldBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR color);
uint8_t LCD_DrawPoint(uint16_t x, uint16_t y, LCD_COLOR color);
uint8_t LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR *pColor);
uint8_t LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ch, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);
uint16_t LCD_ShowString(uint16_t x, uint16_t y, char* pStr, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);


#endif

