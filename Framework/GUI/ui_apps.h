
#ifndef _UI_APPS_H
#define _UI_APPS_H

#include "public.h"
#include "ui_handler.h"


typedef enum
{
    APP_LEVEL_SPLASH = 0,
    APP_LEVEL_TMP,
    APP_LEVEL_USER,
    APP_LEVEL_SYS,

    APP_LEVEL_MAX
}AppLevel;

#if (APP_LEVEL_MAX != UI_LEVEL_MAX)
#error "max app level != max ui level!"
#endif

typedef bool (*AppEvtHandler)(UiEvent *evt, void *arg);
typedef void (*AppUnload)(void);

typedef struct
{
    AppEvtHandler EvtHandler;
    AppUnload     UnloadApp;
}uiApp;



extern uiApp *UiApps[APP_LEVEL_MAX];

extern void UiApp_Init(void);
extern bool UiAppHandlerEvent(UiEvent *evt, void *arg);
extern void UiAppRegister(uiApp *app, AppLevel level);
extern void UiAppDeregister(uiApp *app, AppLevel level);


#endif

