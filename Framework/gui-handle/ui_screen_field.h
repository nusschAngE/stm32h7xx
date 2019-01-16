
#ifndef _UI_SCREEN_FIELD_H
#define _UI_SCREEN_FIELD_H

#include "public.h"
#include "ui_event.h"

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
}ScreenRegion_Typedef;

typedef struct
{
    ScreenRegion_Typedef region;

    bool (*EvtHandler)(TpEvent *evt, void *p_arg);
}TpFiled_Typedef;


/**** PUBLIC FUNCTION *****/
bool UiMatchTpPoint(ScreenRegion_Typedef region, uint16_t tpXpos, uint16_t tpYpos);

#endif //_UI_SCREEN_FIELD_H

