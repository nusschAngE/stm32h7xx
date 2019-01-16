
#ifndef _UI_APPLET_H
#define _UI_APPLET_H

#include "public.h"
#include "ui_support.h"

typedef enum
{
    APP_STATE_IDLE = 0,
    APP_STATE_PLAY,
    APP_STATE_MENU,
}APP_STATE;

typedef struct
{
    EventHandler evtHandler;
    UnloadApp    unload;
}UiApp_Typedef;

extern UiApp_Typedef *UiApp[MAX_UI_LEVEL];

/*  */
extern uint8_t UiAppInit(void);
extern void UiApp_Register(UiApp_Typedef *app, UiLevel level);
extern void UiApp_Deregister(UiApp_Typedef *app, UiLevel level);

#endif


