

#include "lcd.h"
#include "ui_handler.h"
#include "ui_screen.h"
#include "ui_event.h"

/* OS message */
OS_EVENT *UiQueue = (OS_EVENT *)NULL;
UiEvent  *UiEvt[MAX_UI_MESSAGE];
static uiMessage UiMesg[MAX_UI_MESSAGE];

/* display function */
static inline void UiFieldClearDisplay(UiField * inField);
static inline void UiFieldBlockDisplay(UiField * inField);

/* UI Filed handler list */
static const UiFieldHandler UiFiledHandlers[FIELD_TYPE_MAX] = 
{
    {NULL, NULL},
    {UiFieldClearDisplay, NULL},
    {UiFieldBlockDisplay, NULL}
};


/* display function */
static inline void UiFieldClearDisplay(UiField *inField)
{
    UiFiledClear *field = (UiFiledClear *)inField;

    if(IsDisplayTopScreen() == FALSE)   return ;

    lcd_FieldBlock(field->x, field->y, field->w, field->h, field->color);
}


static inline void UiFieldBlockDisplay(UiField *inField)
{
    UiFiledBlock *field = (UiFiledBlock *)inField;

    if(IsDisplayTopScreen() == FALSE)   return ;

    lcd_FieldBlock(field->x, field->y, field->w, field->h, field->color);
}

/* UI Field handler */
bool UiFieldHandleEvent(UiEvent *evt, UiField *inField)
{
    UiFieldType type = FIELD_TYPE_NONE; 
    bool handle = FALSE;

    if(inField)
    {
        type = *(UiFieldType *)inField;
    }
    else
    {
        return (FALSE);
    }

    if(UiFiledHandlers[type].EvtHandler)
    {
        handle = UiFiledHandlers[type].EvtHandler(evt, inField);
    }

    return (handle);
}

void UiFieldDisplay(UiField *inField)
{
    UiFieldType type = FIELD_TYPE_NONE; 

    if(inField)
    {
        type = *(UiFieldType *)inField;
    }
    else
    {
        return ;
    }

    if(UiFiledHandlers[type].display)
    {
        UiFiledHandlers[type].display(inField);
    }
}

void UiHandler_Init(void)
{
    uint8_t i;

    /* create message queue */
    UiQueue = OSQCreate((void**)UiEvt, MAX_UI_MESSAGE);
    /* init message cache */
    for(i = 0; i < MAX_UI_MESSAGE; i++)
    {
        UiMesg[i].empty = TRUE;
    }
}

/* OS message */
bool UiSendIoEvent(IoEvent *evt)
{
    uint8_t i, ret;

    for(i = 0; i < MAX_UI_MESSAGE; i++)
    {
        if(UiMesg[i].empty)
        {
            UiEventFromIO(&(UiMesg[i].evt), evt);
            ret = OSQPost(UiQueue, &UiMesg[i]);
            if(ret == OS_ERR_NONE)
            {
                return (TRUE);
            }
            break;
        }
    }

    return (FALSE);
}

bool UiSendTpEvent(TpEvent *evt)
{
    uint8_t i, ret;

    for(i = 0; i < MAX_UI_MESSAGE; i++)
    {
        if(UiMesg[i].empty)
        {
            UiEventFromTP(&(UiMesg[i].evt), evt);
            ret = OSQPost(UiQueue, &UiMesg[i]);
            if(ret == OS_ERR_NONE)
            {
                return (TRUE);
            }
            break;
        }
    }

    return (FALSE);    
}

bool UiGetUiEvent(UiEvent *evt)
{
    INT8U ret;
    uiMessage *UiMesg;

    UiMesg = (uiMessage *)OSQAccept(UiQueue, &ret);
    if(ret == OS_ERR_NONE)
    {
        evt->code = UiMesg->evt.code;
        evt->data = UiMesg->evt.data;
        UiMesg->empty = TRUE;

        return (TRUE);
    }

    return (FALSE);
}

void UiTpEventGetXY(UiEvent *evt, uint16_t *xPos, uint16_t *yPos)
{
    uint32_t data = (uint32_t)evt->data;

    *xPos = (uint16_t)(data >> 16);
    *yPos = (uint16_t)(data & 0x0000ffff);
}







