
#ifndef _MCU_LCD_H
#define _MCU_LCD_H

#include "public.h"

typedef enum
{
    LCD_OK = 0,
    LCD_BC, //cross the border
    LCD_ERR
}LCDStatus_Enum;

typedef struct
{
    uint32_t id;
    uint8_t  dir;
    uint16_t width;
    uint16_t height;
}LCDDev_Struct;

/* LCD device struct */
extern LCDDev_Struct LCDDevice;

void LCD_Init(void);
void LCD_Onoff(uint8_t on);
void LCD_BacklightOnoff(uint8_t on);
void LCD_SetScanDirection(uint8_t dir);
void LCD_Clear(LCD_COLOR color);
uint8_t LCD_ClearBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR color);
uint8_t LCD_DrawPoint(uint16_t x, uint16_t y, LCD_COLOR color);
uint8_t LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR *pColor);
uint8_t LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ch, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);
uint16_t LCD_ShowString(uint16_t x, uint16_t y, char* pStr, uint8_t font, LCD_COLOR fr, LCD_COLOR bg);

















#endif

