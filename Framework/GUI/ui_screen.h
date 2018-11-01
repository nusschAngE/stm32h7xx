
#ifndef _UI_SCREEN_H
#define _UI_SCREEN_H

#include "public.h"
#include "ui_handler.h"

typedef enum
{
    UI_LEVEL_SPLASH = 0,
    UI_LEVEL_TMP,
    UI_LEVEL_USER,
    UI_LEVEL_SYS,

    UI_LEVEL_MAX
}UiLevel;

typedef struct
{
    UiField **fields;
    uint16_t touchID;
    uint8_t fieldNo;
}UiScreen;

extern UiScreen *ScreenList[UI_LEVEL_MAX];

extern void UiScreen_Init(void);
extern bool UiScreenHandleEvent(UiEvent *evt, void *arg);
extern void UiScreenRegister(UiScreen *screen, UiLevel level);
extern void UiScreenDeregister(UiScreen *screen, UiLevel level);
extern void UiScreenRefresh(void);
extern bool IsDisplayTopScreen(void);

#endif


