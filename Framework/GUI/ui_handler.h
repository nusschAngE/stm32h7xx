
#ifndef _UI_HANDLER_H
#define _UI_HANDLER_H

#include <ucos_ii.h>

#include "public.h"
#include "ui_event.h"
#include "lcd.h"

/* OS message */
#define MAX_UI_MESSAGE      (32)

typedef bool (*UiEvtHandler)(UiEvent *evt, void *arg);

typedef struct
{
    bool    empty;
    
    UiEvent evt;    
}uiMessage;

typedef uint8_t         UiFieldType;
typedef void            UiField;

typedef struct
{
    void (*display)(UiField *field);
    UiEvtHandler    EvtHandler;
}UiFieldHandler;

typedef enum
{
    FIELD_TYPE_NONE = 0,
    FIELD_TYPE_CLEAR,
    FIELD_TYPE_BLOCK,
    FIELD_TYPE_ICON,
    FIELD_TYPE_PICTURE,
    FIELD_TYPE_TEXT,

    FIELD_TYPE_MAX
}UiFiledType;

/*  field type clear
*/
typedef struct
{
    UiFiledType Type;
    LCD_COLOR   color;  
    
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;          
}UiFiledClear;

#define UI_FILED_CLEAR(x, y, w, h, color)\
    {FIELD_TYPE_CLEAR, color, x, y, w, h}


/*  field type block
*/
typedef struct
{
    UiFiledType Type;
    LCD_COLOR   color;  
    
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;          
}UiFiledBlock;

#define UI_FILED_BLOCK(x, y, w, h, color)\
    {FIELD_TYPE_BLOCK, color, x, y, w, h}





/* GLOBAL FUNCTION */
/* UI Field handler */
extern void UiHandler_Init(void);
extern bool UiFieldHandleEvent(UiEvent *evt, UiField *inField);
extern void UiFieldDisplay(UiField *inField);
/* OS message */
extern bool UiSendIoEvent(IoEvent *evt);
extern bool UiSendTpEvent(TpEvent *evt);
extern bool UiGetUiEvent(UiEvent *evt);
extern void UiTpEventGetXY(UiEvent *evt, uint16_t *xPos, uint16_t *yPos);
#endif


