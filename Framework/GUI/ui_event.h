
#ifndef _UI_EVENT_H_
#define _UI_EVENT_H_

#include <stdint.h>

/* mask */
#define UI_EVT_GROUP_MASK           ((uint32_t)0xFF000000)
#define UI_EVT_INDEX_MASK           ((uint32_t)0x00FFFF00)
#define UI_EVT_FLAG_MASK            ((uint32_t)0x000000FF)

/* event group */
#define UI_EVT_GROUP_SYS            ((uint32_t)0x01000000)
#define UI_EVT_GROUP_IO             ((uint32_t)0x02000000)
#define UI_EVT_GROUP_TP             ((uint32_t)0x03000000)


/* get event */
#define UI_EVT_SYS(evt)             (UI_EVT_GROUP_SYS + ((uint32_t)evt << 8))
#define UI_EVT_IO(evt, flag)        (UI_EVT_GROUP_IO + ((uint32_t)evt << 8) + (uint32_t)flag)
#define UI_EVT_TP_PRESS(flag)       (UI_EVT_GROUP_TP + (uint32_t)flag)
#define UI_EVT_TP_GESTURE(gust)     (UI_EVT_GROUP_TP + ((uint32_t)gust << 8))

/* ui event struct */
typedef struct
{
    uint32_t code;
    void *data;
}UiEvent;

/* io event */
typedef struct
{
    uint16_t code;
    uint16_t data;
    uint8_t flag;
}IoEvent;

/* tp event */
typedef struct
{
    uint16_t xPos;
    uint16_t yPos;
    uint8_t flag;
    uint8_t point;
    uint8_t gesture;
}TpEvent;

extern uint32_t UiEventGetCode(UiEvent *evt);
extern uint8_t UiEventGetFlag(UiEvent *evt);
extern void UiEventFromIO(UiEvent *uiEvt, IoEvent *ioEvt);
extern void UiEventFromTP(UiEvent *uiEvt, TpEvent *tpEvt);

#endif

