
#ifndef _FONT_H_
#define _FONT_H_

#include "public.h"

#define FontPreset(x)   GetFontPreset(x)

/* font color
*/
#define FONT_FG_BLACK   ((LCD_COLOR)0x0000)
#define FONT_FG_WHITE   ((LCD_COLOR)0xFFFF)

#define FONT_BG_BLACK   ((LCD_COLOR)0x0000)
#define FONT_BG_WHITE   ((LCD_COLOR)0xFFFF)

/* Font Size
*/
typedef enum
{
    FONT_SIZE_12 = 0,
    FONT_SIZE_16,
    FONT_SIZE_24,
    FONT_SIZE_32,

    FONT_SIZE_UN,
}FontSize;

/* Font Type
*/
typedef enum
{
    FONT_TYPE_HZK = 0,
    FONT_TYPE_TTF,

    FONT_TYPE_UN
}FontType;

/* Font Background Type
*/
typedef enum
{
    FONT_BG_COLOR = 0,
    FONT_BG_PIC,

    FONT_BG_UN
}FontBG;

/* font info
*/
typedef struct
{
    uint8_t   size;     /* font size */
    LCD_COLOR fg;       /* foreground color */
    LCD_COLOR bg;       /* background color */
}Font_Typedef;

/* hzk font info
*/
typedef struct
{
    uint8_t Font[4];//"hzk "

    uint32_t unigbkAddr;
    uint32_t unigbkSize;
    uint32_t hzk12Addr; //start address in SPIFlash
    uint32_t hzk12Size; //total GBK size
    uint32_t hzk16Addr;
    uint32_t hzk16Size;
    uint32_t hzk24Addr;
    uint32_t hzk24Size;
    uint32_t hzk32Addr;
    uint32_t hzk32Size;
}HzkFontInfo_Typedef;


/* font in flash */
extern HzkFontInfo_Typedef HzkFontInfo;

extern uint8_t HZKFont_Check(void);
extern Font_Typedef GetFontPreset(uint8_t preset);














#endif

