
#ifndef _ICON_H
#define _ICON_H

#include "public.h"


/* ICON List */
typedef enum
{
    ICON_DEFAULT = 0,
    /* APP entry icon */
    ICON_APP_MB_SELECTED,
    ICON_APP_MB_UNSELECTED,

    ICON_SRC_MAX
}ICON_SRC;

typedef struct
{
    uint32_t addr;//address in flash
    uint32_t size;//number bytes of icon source
    uint16_t w;
    uint16_t h;
}ICON_SourceTypedef;







/**** PUBLIC FUNCTION ****/
extern void Icon_DrawScreen(uint16_t x, uint16_t y, ICON_SRC icon);

extern void Icon_DrawGRAM(uint16_t x, uint16_t y, ICON_SRC icon);


#endif


