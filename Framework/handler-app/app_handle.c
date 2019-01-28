

#include "ucos_ii.h"
#include "ui_event.h"
#include "app_handle.h"


AppHandle_t *AppHandler[MAX_APP_LEVEL];

uint8_t AppsInit(void)
{
    AppLevel_t i = 0;

    for(i = 0; i < MAX_APP_LEVEL; i++)
    {
        AppHandler[i] = NULL;
    }

    return (0);
}

void App_Register(AppHandle_t *app, AppLevel_t level)
{
    if((AppHandler[level] != NULL) 
        && (AppHandler[level] != app))
    {
        if(AppHandler[level]->Unload != NULL)
        {
            AppHandler[level]->Unload();
        }        
    }
    AppHandler[level] = app;
}

void App_Deregister(AppHandle_t *app, AppLevel_t level)
{
    if(AppHandler[level] == app)
    {        
        AppHandler[level] = NULL;
    }
}


/* applet task body */
void app_handler(void *p_arg)
{
    UiEvent_t uiEvt;
    AppLevel_t appLevel = 0;
    bool handled = FALSE;
	
    while(1)
    {
        if(UiGetUiEvt(&uiEvt) == 0)//error none
        {
        	//printf("ui event handle\r\n");
        	handled = FALSE;
        #if 1
            for(appLevel = 0; (appLevel<MAX_APP_LEVEL)&&(!handled); appLevel++)
            {
                if((AppHandler[appLevel] != NULL) 
                	&& (AppHandler[appLevel]->EvtHandler != NULL))
                {
                	//printf("app handler\r\n");
                    handled = AppHandler[appLevel]->EvtHandler(&uiEvt, NULL);
                }
            }
		#else
			handled = App[UI_LEVEL_PLAY]->EvtHandler(&uiEvt, NULL);
		#endif
        }
    
        OSTimeDlyHMSM(0, 0, 0, 5);
    }
}

