
#include "stm32h7xx.h"

#include "led.h"

#include "ucos_ii.h"
#include "fatfs_ex.h"

#include "ui_event.h"
#include "ui_applet.h"

/* STATIC FUNC */
static void MusicPlayer_UnloadApp(void);
static bool MusicPlayer_EventHandler(UiEvent *evt, void *p_arg);

/* APP Content */
UiApp_Typedef MusicPlayerApp = {MusicPlayer_EventHandler, MusicPlayer_UnloadApp};

static uint8_t MusicPlayerState = APP_STATE_IDLE;

/********** PUBLIC FUNCTION ************/

void MusicPlayer_LoadApp(void)
{
}

/********** PRIVATE FUNCTION ************/
static void MusicPlayer_UnloadApp(void)
{

}

static bool MusicPlayer_EventHandler(UiEvent *evt, void *p_arg)
{
    (void)p_arg;
    bool handled = TRUE;

    switch(evt->code)
    {
        case UI_EVT_IO(IO_EVT_UP, IO_KEY_DOWN)://use for 'LIST'
            {
            }
            break;
        case UI_EVT_IO(IO_EVT_DOWN, IO_KEY_DOWN)://use for 'PLAY/PAUSE'
            {
            }
            break;
        case UI_EVT_IO(IO_EVT_LEFT, IO_KEY_DOWN)://use for 'PREVIOUS'
            {
            }
            break;
        case UI_EVT_IO(IO_EVT_RIGHT, IO_KEY_DOWN)://use for 'NEXT'
            {
            }
            break;
        case UI_EVT_SYS(SYS_EVT_TICK_10MS):
            LED_Toggle(LED_RED);
            break;
        case UI_EVT_SYS(SYS_EVT_TICK_1S):
            LED_Toggle(LED_GREEN);
            break;
        default:
            break;
    }
    
    return (handled);
}

