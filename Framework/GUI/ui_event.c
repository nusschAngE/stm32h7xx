
#include <ucos_ii.h>
#include "public.h"
#include "ui_event.h"

/*************** PUBLIC FUNCTION ******************/
uint32_t UiEventGetCode(UiEvent *evt)
{
    return evt->code;
}

uint8_t UiEventGetFlag(UiEvent *evt)
{
    return (uint8_t)(evt->code & UI_EVT_FLAG_MASK);
}

void UiEventFromIO(UiEvent *uiEvt, IoEvent *ioEvt)
{
    uint32_t data = ioEvt->data;
    
    uiEvt->code = UI_EVT_IO(ioEvt->code, ioEvt->flag);
    uiEvt->data = (void *)data;
}

void UiEventFromTP(UiEvent *uiEvt, TpEvent *tpEvt)
{
    uint32_t data = (((uint32_t)tpEvt->xPos) << 16) + (uint32_t)tpEvt->yPos;
    
    uiEvt->code = UI_EVT_TP_PRESS(tpEvt->flag) | UI_EVT_TP_GESTURE(tpEvt->gesture);
    uiEvt->data = (void *)data;
}



