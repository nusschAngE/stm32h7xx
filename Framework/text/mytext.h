
#ifndef _MY_TEXT_H
#define _MY_TEXT_H

#include "public.h"
#include "font.h"



/* PUBLIC FUNC
*/
extern void ShowTextLineGbk(uint16_t x, uint16_t y, const uint8_t *pText, FONT_t font);
extern void ShowTextLineUnicode(uint16_t x, uint16_t y, const uint8_t *pText, FONT_t font);
extern void ShowTextLineAscii(uint16_t x, uint16_t y, const char *pText, FONT_t font);
extern uint16_t TextDispWidth(uint8_t *pText, uint8_t fontSize);
extern uint16_t TextDispHeight(uint8_t *pText, uint8_t fontSize);
#endif

