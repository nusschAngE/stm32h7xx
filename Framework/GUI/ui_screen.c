
#include <ucos_ii.h>

#include "lcd.h"
#include "iokpd_track.h"
#include "irkpd_track.h"

#include "ui_screen.h"
#include "ui_event.h"
#include "ui_apps.h"
#include "ui_handler.h"

UiScreen *ScreenList[UI_LEVEL_MAX];
static uint8_t UiTopLevel = 0, UiDispLevel = 0;

void UiScreen_Init(void)
{
    
}

bool UiScreenHandleEvent(UiEvent *evt, void *arg)
{
    uint8_t level = 0, i;
    bool handled = FALSE;

    for(level= 0; level < UI_LEVEL_MAX; level++)
    {
        if(ScreenList[level] != NULL)
        {
            for(i = 0; i < ScreenList[level]->fieldNo; i++)
            {
                handled = UiFieldHandleEvent(evt, ScreenList[level]->fields[i]);
                //evt not pass
                if(handled) break;
            }
        }
    }
		return (handled);
}

void UiScreenRegister(UiScreen *screen, UiLevel level)
{
    ScreenList[level] = screen;
}

void UiScreenDeregister(UiScreen *screen, UiLevel level)
{
    if(ScreenList[level] == screen)
    {
        ScreenList[level] = NULL;
    }
}

/* UI Display refresh */
void UiScreenRefresh(void)
{
    uint8_t i;
    
    for(UiDispLevel = 0; UiDispLevel < UI_LEVEL_MAX; UiDispLevel++)
    {
        if(ScreenList[UiDispLevel] != NULL) break;
    }

    UiTopLevel = UiDispLevel;

    for(UiDispLevel = 0; UiDispLevel < UI_LEVEL_MAX; UiDispLevel++)
    {
        for(i = 0; i < ScreenList[UiDispLevel]->fieldNo; i++)
        {
            UiFieldDisplay(ScreenList[UiDispLevel]->fields[i]);
        }
    }
}

bool IsDisplayTopScreen(void)
{
    return (UiTopLevel == UiDispLevel) ? TRUE : FALSE;
}

