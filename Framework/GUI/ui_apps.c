
#include "stm32h7xx.h"
#include "ui_event.h"
#include "ui_apps.h"

uiApp *UiApps[APP_LEVEL_MAX];


/**************** GLOBAL FUNCTION ****************/

void UiApp_Init(void)
{
    
}

bool UiAppHandlerEvent(UiEvent *evt, void *arg)
{
    uint8_t level = 0;
    bool handled = FALSE;

    for(level = 0; level < APP_LEVEL_MAX; level++)
    {
        if(UiApps[level]->EvtHandler)
        {
            handled = UiApps[level]->EvtHandler(evt, arg);
        }
    }

    return handled;
}


void UiAppRegister(uiApp *app, AppLevel level)
{
    /* unload same level app */
    if((UiApps[level] != NULL) && (UiApps[level] != app)
        && (UiApps[level]->UnloadApp != NULL))
    {
        UiApps[level]->UnloadApp();
    }

    UiApps[level] = app;
}

void UiAppDeregister(uiApp *app, AppLevel level)
{
    if(UiApps[level] == app)
    {
        UiApps[level] = NULL;
    }
}


