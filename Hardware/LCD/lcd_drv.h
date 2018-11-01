
#ifndef _LCD_DRV_H_
#define _LCD_DRV_H_

#include "public.h"

/* LCD size indicate in LCD's datasheet */
#define DEFAULT_LCD_WIDTH       ((uint16_t)480)
#define DEFAULT_LCD_HEIGHT      ((uint16_t)800)

enum
{
    LCD_OK = 0,
    LCD_BC, //cross the border
    LCD_ERR
};

typedef struct
{
    uint32_t id;
    uint8_t  dir;
    uint16_t width;
    uint16_t height;
}_lcd_dev;

/* LCD device struct */
extern _lcd_dev lcdDev;

void lcd_Init(void);
void lcd_SetOnOff(uint8_t on);
void lcd_SetBacklightOnOff(uint8_t on);
void lcd_SetScanDirection(uint8_t dir);
void lcd_clear(LCD_COLOR color);
uint8_t lcd_ClearBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR color);
uint8_t lcd_DrawPoint(uint16_t x, uint16_t y, LCD_COLOR color);
uint8_t lcd_ShowPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR *pColor);
uint8_t lcd_ShowChar(uint16_t x, uint16_t y, uint8_t ch, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);
uint16_t lcd_ShowString(uint16_t x, uint16_t y, char* pStr, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);



#endif

