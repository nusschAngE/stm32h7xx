
#ifndef _TEXT_H
#define _TEXT_H

#include "public.h"
#include "font.h"


/* text info
*/
typedef struct
{
    uint16_t   flags;
    uint16_t   idx; //text index in preset list
    uint8_t    enc; //encode
    const uint8_t *pStr;
}Text_Typedef;

/* text work region
*/
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
}TextWorkRegion_Typedef;

/* PUBLIC FUNC
*/
extern void ShowTextLineGbk(uint16_t x, uint16_t y, const uint8_t *pStr, Font_Typedef font);
extern void ShowTextLineUnicode(uint16_t x, uint16_t y, const uint8_t *pStr, Font_Typedef font);
extern void ShowTextLineAscii(uint16_t x, uint16_t y, const char *pStr, Font_Typedef font);
extern void ShowText(TextWorkRegion_Typedef region, Text_Typedef text, Font_Typedef font);

#endif

