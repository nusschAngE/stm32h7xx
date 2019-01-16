
#ifndef _LCD_GRAM_H
#define _LCD_GRAM_H

#include "public.h"

/* PUBLIC FUNTION */
void LCDGram_FillBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, LCD_COLOR color);
void LCDGram_FillPicture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, LCD_COLOR *pic);
uint8_t LCDGram_DisplayInit(void);
void LCDGram_FrameDisplay(void);

#endif

