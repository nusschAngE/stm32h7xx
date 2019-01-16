
#include "stm32h7xx.h"

#include "led.h"
#include "lcd.h"

#include "ucos_ii.h"
#include "fatfs_ex.h"
#include "icon.h"

#include "ui_applet.h"
#include "ui_screen_field.h"
#include "base_home.h"
#include "app_move_block.h"

/* STATIC FUNC */
static void Home_UnloadApp(void);
static void Home_ScreenShow(void);
static bool Home_EventHandler(UiEvent *evt, void *p_arg);
static bool Home_TpEventHandler(TpEvent *evt, void *p_arg);

/* app entry list */
static const AppEntry_Typedef AppEntryList[] = 
{
    /* move block app */
    {
        ICON_APP_MB_SELECTED,
        ICON_APP_MB_UNSELECTED,
        "Move Block",
        MoveBlock_LoadApp
    },
    /* music player app */
    
};

#define HOME_APPENTRY_NBR       (sizeof(AppEntryList)/sizeof(AppEntry_Typedef))

/* tp handler list */
static const TpFiled_Typedef HomeTpField[] = 
{
    /* move block app icon's tp handler */
    {
        {40, 40, 48, 48},
        Home_TpEventHandler,
    },
};

#define HOME_TPFIELD_NBR        (sizeof(HomeTpField)/sizeof(TpFiled_Typedef))

/* APP Content */
UiApp_Typedef HomeApp = {Home_EventHandler, Home_UnloadApp};



/******** PUBLIC FUNCTION ***********/
void Home_LoadApp(void)
{
    Home_ScreenShow();

    UiApp_Register(&HomeApp, UI_LEVEL_PLAY);
}

/******** PRIVATE FUNCTION ***********/
static void Home_UnloadApp(void)
{
    LCD_Clear(COLOR_BLACK);

    UiApp_Deregister(&HomeApp, UI_LEVEL_PLAY);
}

static bool Home_EventHandler(UiEvent *evt, void *p_arg)
{
    (void)p_arg;
    bool handled = TRUE;

    switch (evt->code)
    {
        case UI_EVT_TP_PRESS(TP_ACT_DOWN):
            {
                uint32_t i = 0;
                TpEvent tpEvent;
                bool match = FALSE;
                
                TpEvtFromUI(evt, &tpEvent);
                //printf("tpEvt[%d-%d]\r\n", tpEvent.xPos, tpEvent.yPos);
                for(i=0; i<HOME_TPFIELD_NBR; i++)
                {
                    match = UiMatchTpPoint(HomeTpField[i].region, tpEvent.xPos, tpEvent.yPos);
                    if(match)
                    {
                    	//printf("point matched!!\r\n");
                        if(HomeTpField[i].EvtHandler != NULL)
                        {
                            HomeTpField[i].EvtHandler(&tpEvent, (void*)i);
                        }
                        break;
                    }
                }
            }
            break;
    }

    return (handled);
}

static bool Home_TpEventHandler(TpEvent *evt, void *p_arg)
{
    uint16_t index = (uint16_t)p_arg;
    bool handled = FALSE;

    if(AppEntryList[index].loadapp != NULL)
    {   
    	printf("load app[%d]\r\n", index);
        AppEntryList[index].loadapp(NULL);
        handled = TRUE;
    }

    return (handled);
}

static void Home_ScreenShow(void)
{
    uint16_t i = 0;
    uint16_t x = 40, y = 40;

    LCD_Clear(0x2f2f);
    
    for(i=0; i<HOME_APPENTRY_NBR; i++)
    {
        Icon_DrawScreen(x, y, AppEntryList[i].appicon_sl);

        x += 48+20;
        if(x > lcdDev.width)
        {
            x = 40;
            y += 48+20;
        }
    }
}

