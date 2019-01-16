

#include "ucos_ii.h"
#include "ui_event.h"
#include "ui_applet.h"
#include "ui_screen.h"
#include "gui_app_handle.h"

/* applet task body */
void app_HandleTask(void *p_arg)
{
    UiEvent uiEvt;
    uint8_t uiLevel = 0;
    bool handled = FALSE;
	
    while(1)
    {
        if(UiGetUiEvt(&uiEvt) == 0)//error none
        {
        	//printf("ui event handle\r\n");
        	handled = FALSE;
        #if 1
            for(uiLevel = 0; (uiLevel<MAX_UI_LEVEL)&&(!handled); uiLevel++)
            {
                if((UiApp[uiLevel] != NULL) 
                	&& (UiApp[uiLevel]->evtHandler != NULL))
                {
                	//printf("app handler\r\n");
                    handled = UiApp[uiLevel]->evtHandler(&uiEvt, NULL);
                }
            }
		#else
			handled = UiApp[UI_LEVEL_PLAY]->evtHandler(&uiEvt, NULL);
		#endif
        }
    
        OSTimeDlyHMSM(0, 0, 0, 5);
    }
}

