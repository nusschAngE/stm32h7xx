
#include "stm32h7xx.h"

#include "led.h"
#include "tft_lcd.h"

#include "ucos_ii.h"
#include "fatfs_ex.h"
#include "icon.h"
#include "mytext.h"
#include "GUI.h"

#include "app_handle.h"
#include "app_base_home.h"
#include "app_move_block.h"
#include "app_music_player.h"

/* STATIC FUNC */
static void Home_UnloadApp(void);
static void Home_LoadScreen(void);
static bool Home_EvtHandler(UiEvent_t *evt, void *p_arg);

#define MP_BKCOLOR_0    (0x4F4F)
#define MP_BKCOLOR_1    (0x4A4A)


/* layout */
#define HOME_LAYOUT_X   (12)
#define HOME_LAYOUT_Y   (24)

#define APP_ICON_W      (48)
#define APP_ICON_H      (48)
#define APP_NAME_FONT   (FONT_SIZE_16)
#define APP_SEPT_H      (6)

/* app entry list */
static const AppEntry_t AppEntryList[] = 
{
    /* move block app */
    {
        ICON_APP_MB_SELECTED,
        ICON_APP_MB_UNSELECTED,
        "Move Block",
        MB_LoadApp,
        NULL
    },
    /* music player app */
    {
        ICON_APP_MB_SELECTED,
        ICON_APP_MB_UNSELECTED,
        "Music Player",
        MP_LoadApp,
        NULL
    }
};

#define HOME_APPENTRY_NBR       (sizeof(AppEntryList)/sizeof(AppEntry_t))

/* APP Content */
AppHandle_t HomeApp = {Home_EvtHandler, Home_UnloadApp};

/******** PUBLIC FUNCTION ***********/
void Home_LoadApp(void)
{
    Home_LoadScreen();

    App_Register(&HomeApp, APP_LEVEL_PLAY);
}

/******** PRIVATE FUNCTION ***********/
static void Home_UnloadApp(void)
{
    TFTLCD_Clear(COLOR_BLACK);

    App_Deregister(&HomeApp, APP_LEVEL_PLAY);
}

static bool Home_EvtHandler(UiEvent_t *evt, void *p_arg)
{
    (void)p_arg;
    bool handled = TRUE;

    switch (evt->code)
    {
        case UI_EVT_TP_PRESS(TP_ACT_DOWN):
            {
                
            }
            break;
    }

    return (handled);
}

static bool Home_TpEventHandler(TpEvent_t *evt, void *p_arg)
{
    uint32_t index = (uint32_t)p_arg;
    bool handled = FALSE;

    if(AppEntryList[index].loadapp != NULL)
    {   
    	printf("load app[%d]\r\n", index);
        AppEntryList[index].loadapp(NULL);
        handled = TRUE;
    }

    return (handled);
}

static void Home_LoadScreen(void)
{
    int xSize;
    int ySize;

    /* Draw back ground */
    xSize = LCD_GetXSize();
    ySize = LCD_GetYSize();
    GUI_DrawGradientV(0, 0, xSize, ySize, MP_BKCOLOR_0, MP_BKCOLOR_1);

    GUI_FillRect(40, 40, 88, 88);
}

