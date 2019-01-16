
#include "ucos_ii.h"
#include "ui_event.h"

/***************/
/* OS message */
OS_EVENT *UiQueue = (OS_EVENT *)NULL;
UiEvent  *UiEvt[MAX_UI_MESG];
static uiMessage UiMesg[MAX_UI_MESG];

/*************** PUBLIC FUNCTION ******************/

uint8_t UiEvtInit(void)
{
	uint8_t i;

	for(i = 0; i < MAX_UI_MESG; i++)
	{
		UiMesg[i].empty = TRUE;
	}

    UiQueue = OSQCreate((void**)UiEvt, MAX_UI_MESG);
    /* done */
    if(UiQueue != (OS_EVENT *)NULL) {return (0);}
	
    return (1);//error
}

uint8_t UiSendIoEvt(IoEvent *evt)
{
    uint8_t i, ret;

    for(i = 0; i < MAX_UI_MESG; i++)
    {
        if(UiMesg[i].empty)
        {
            UiEvtFromIO(&(UiMesg[i].evt), evt);
            ret = OSQPost(UiQueue, &UiMesg[i]);
            /* done */
            if(ret == OS_ERR_NONE) 
            {
                UiMesg[i].empty = FALSE;
                return (0);
            }
        }
    }
    
    return (1);//error
}

uint8_t UiSendTpEvt(TpEvent *evt)
{
    uint8_t i, ret;

    for(i = 0; i < MAX_UI_MESG; i++)
    {
        if(UiMesg[i].empty)
        {
            UiMesg[i].empty = FALSE;
            UiEvtFromTP(&(UiMesg[i].evt), evt);
            ret = OSQPost(UiQueue, &UiMesg[i]);
            /* done */
            if(ret == OS_ERR_NONE) {return (0);}
        }
    }
    return (1);//error    
}

uint8_t UiSendSysEvt(SysEvent *evt)
{
    uint8_t i, ret;

    for(i = 0; i < MAX_UI_MESG; i++)
    {
        if(UiMesg[i].empty)
        {
            UiEvtFromSYS(&(UiMesg[i].evt), evt);
            ret = OSQPost(UiQueue, &UiMesg[i]);
            /* done */
            if(ret == OS_ERR_NONE) 
            {
                UiMesg[i].empty = FALSE;
                return (0);
            }
        }
    }
    return (1);//error
}

uint8_t UiGetUiEvt(UiEvent *evt)
{
    INT8U ret;
    uiMessage *UiMesg;

    UiMesg = (uiMessage *)OSQAccept(UiQueue, &ret);
    if(ret == OS_ERR_NONE)
    {
        evt->code = UiMesg->evt.code;
        evt->data = UiMesg->evt.data;
        UiMesg->empty = TRUE;

        return (0);
    }
    //printf("UiGetUiEvt() error\r\n");
    return (1);//error
}

void UiEvtGetXY(UiEvent *evt, uint16_t *xPos, uint16_t *yPos)
{
    uint32_t data = (uint32_t)evt->data;

    *xPos = (uint16_t)((data >> 16) & 0x00007fff);
    *yPos = (uint16_t)(data & 0x00007fff);
}

uint32_t UiEvtGetCode(UiEvent *evt)
{
    return evt->code;
}

uint8_t UiEvtGetFlag(UiEvent *evt)
{
    return (uint8_t)(evt->code & UI_EVT_FLAG_MASK);
}

int32_t UiEvtGetData(UiEvent *uiEvt)
{
    return uiEvt->data;
}

void UiEvtFromIO(UiEvent *uiEvt, IoEvent *ioEvt)
{    
    uiEvt->code = UI_EVT_IO(ioEvt->code, ioEvt->flag);
    uiEvt->data = ioEvt->data;
}

void UiEvtFromTP(UiEvent *uiEvt, TpEvent *tpEvt)
{
    int32_t data = (((int32_t)tpEvt->xPos) << 16) + (int32_t)tpEvt->yPos;
    
    uiEvt->code = UI_EVT_TP_PRESS(tpEvt->flag) | UI_EVT_TP_GESTURE(tpEvt->gesture);
    uiEvt->data = data;
}

void UiEvtFromSYS(UiEvent *uiEvt, SysEvent *sysEvt)
{
    uiEvt->code = UI_EVT_SYS(sysEvt->code);
    uiEvt->data = sysEvt->data;
}

void TpEvtFromUI(UiEvent *uiEvt, TpEvent *tpEvt)
{
    UiEvtGetXY(uiEvt, &tpEvt->xPos, &tpEvt->yPos);

    tpEvt->flag = uiEvt->code & TP_EVT_FLAG_MASK;
    tpEvt->gesture = uiEvt->code & TP_EVT_GESTRUE_MASK;
}

