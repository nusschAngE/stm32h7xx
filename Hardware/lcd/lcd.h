
#ifndef _LCD_H
#define _LCD_H

#include "public.h"

/* Tx-Cplt call back */
typedef void(*XsferCplt)(void);

/* LCD default size */
#define DEFAULT_LCD_WIDTH       ((uint16_t)480)
#define DEFAULT_LCD_HEIGHT      ((uint16_t)800)

#define COLOR_BLACK             ((LCD_COLOR)0x0000)
#define COLOR_WHITE             ((LCD_COLOR)0xFFFF)

/* LCD fmc */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
}LCD_Typedef;

/* FMC_A18 -> LCD_RS 
*  FMC_NE1 -> LCD_CS 
*/
#define LCD_BASE    ((uint32_t)0x60000000 | 0x0007fffe)
#define LCD         ((LCD_Typedef*)LCD_BASE)

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
uint8_t LCD_Init(void);
void LCD_SetDispOnOff(bool on);
void LCD_SetBkliteOnOff(bool on);
void LCD_SetScanDirection(uint8_t dirt);
void LCD_Clear(LCD_COLOR color);
void LCD_SetWorkRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_StartDataSession(void);
void LCD_FlushData(LCD_COLOR *pData, uint32_t size);
void LCD_FillBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR color);
void LCD_DrawPoint(uint16_t x, uint16_t y, LCD_COLOR color);
void LCD_FillPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, LCD_COLOR *pColor);

/* LCD DMA Functions */
uint8_t LCD_DMAChannelInit(XsferCplt xsferCpltFunc);
void LCD_DMAXsferStart(LCD_COLOR *pData, uint32_t size);
void LCD_DMAXsferStop(void);

#endif

