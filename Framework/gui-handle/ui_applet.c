
#include "ui_applet.h"


UiApp_Typedef *UiApp[MAX_UI_LEVEL];

uint8_t UiAppInit(void)
{
    UiLevel i = 0;

    for(i = 0; i < MAX_UI_LEVEL; i++)
    {
        UiApp[i] = NULL;
    }

    return (0);
}

void UiApp_Register(UiApp_Typedef *app, UiLevel level)
{
    if((UiApp[level] != NULL) 
    	&& (UiApp[level] != app))
    {
        if(UiApp[level]->unload != NULL)
        {
            UiApp[level]->unload();
        }        
    }
    UiApp[level] = app;
}

void UiApp_Deregister(UiApp_Typedef *app, UiLevel level)
{
    if(UiApp[level] == app)
    {        
        UiApp[level] = NULL;
    }
}




