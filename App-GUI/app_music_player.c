
#include "stm32h7xx.h"

#include "led.h"
#include "tft_lcd.h"

#include "mytext.h"
#include "ucos_ii.h"
#include "fatfs_ex.h"

#include "app_handle.h"
#include "GUI.h"

#define MP_LAYOUT_X         (8)
#define MP_LAYOUT_Y         (24)
#define MP_LAYOUTFONT_H     (16)

#define MUSIC_COVER_X       (MP_LAYOUT_X + 80)
#define MUSIC_COVER_Y       (MP_LAYOUT_Y + 2)
#define MUSIC_COVER_W       (320)
#define MUSIC_COVER_H       (320)

#define MP_TITLE_X          (MP_LAYOUT_X + 8)
#define MP_TITLE_Y          (MUSIC_COVER_Y + MUSIC_COVER_H + 4)

#define MP_ARTIST_X         (MP_LAYOUT_X + 8)
#define MP_ARTIST_Y         (MP_TITLE_Y + MP_LAYOUTFONT_H + 1)

#define MP_PROGBAR_X        (MP_LAYOUT_X)
#define MP_PROGBAR_Y        (MP_ARTIST_Y + MP_LAYOUTFONT_H + 4)
#define MP_PROGBAR_W        (320)

#define AUDIO_BUFF_SIZE     (8*1024)
#define MPTEMP_BUFF_SIZE    (8*1024)

typedef struct
{
    TCHAR path[256];
    TCHAR name[512];
}FileName_t;

typedef struct
{
    FileName_t *name[1024];
    uint16_t listSize;
}FileList_t;

/* STATIC FUNC */
static void MP_UnloadApp(void);
static bool MP_EvtHandler(UiEvent_t *evt, void *p_arg);
static void MP_TrackPlay(int track);
static void MP_PlayPause(bool on);
static void MP_RefreshFileList(FileList_t *list);

/* GUI Layout */
static void MP_LoadPlayScreen(void);
static void MP_LoadFileListScreen(void);
static void MP_RefreshScreen(void);

/* APP Content */
AppHandle_t MusicPlayerApp = {MP_EvtHandler, MP_UnloadApp};

/* audio buffer */
//static FIL MFile;
//static uint8_t *mpAudBuffer0;
//static uint8_t *mpAudBuffer1;
//static uint8_t *mpTempBuffer;
/* play info */
//static uint16_t currMpTrack = 0;
//static uint16_t totalSecond = 0;
//static uint16_t currSecond = 0;

static FileList_t MP_FileList;
static int  CurrTrack = -1;
static bool CurrPlaying = FALSE;
static uint8_t CurrScreen = 0;//0:play 1:file list

/********** PUBLIC FUNCTION ************/

void MP_LoadApp(void *p_arg)
{
    (void)p_arg;

    MP_LoadPlayScreen();
    CurrScreen = 0;
    //MP_LoadFileListScreen();
    //CurrScreen = 1;
    MP_RefreshFileList(&MP_FileList);
    
    App_Register(&MusicPlayerApp, APP_LEVEL_PLAY);
}

/********** PRIVATE FUNCTION ************/
static void MP_UnloadApp(void)
{
    TFTLCD_Clear(COLOR_BLACK);
    App_Deregister(&MusicPlayerApp, APP_LEVEL_PLAY);
}

static bool MP_EvtHandler(UiEvent_t *evt, void *p_arg)
{
    (void)p_arg;
    bool handled = TRUE;

    switch(evt->code)
    {
        case UI_EVT_IO(IO_EVT_UP, IO_KEY_DOWN)://use for 'LIST'
            {
                if(CurrScreen == 0)//play
                {
                    MP_LoadFileListScreen();
                    CurrScreen = 1;
                }

                if(CurrScreen == 1)//file list
                {
                    MP_LoadPlayScreen();
                    CurrScreen = 0;
                }
            }
            break;
        case UI_EVT_IO(IO_EVT_DOWN, IO_KEY_DOWN)://use for 'PLAY/PAUSE'
            {
                CurrPlaying ^= TRUE;
                MP_PlayPause(CurrPlaying);
            }
            break;
        case UI_EVT_IO(IO_EVT_LEFT, IO_KEY_DOWN)://use for 'PREVIOUS'
            {
                MP_TrackPlay(--CurrTrack);
            }
            break;
        case UI_EVT_IO(IO_EVT_RIGHT, IO_KEY_DOWN)://use for 'NEXT'
            {
                MP_TrackPlay(++CurrTrack);
            }
            break;
        case UI_EVT_SYS(SYS_EVT_TICK_10MS):
            MP_RefreshScreen();
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

static void MP_TrackPlay(int track)
{
    
}

static void MP_PlayPause(bool on)
{
    
}

static void MP_RefreshFileList(FileList_t *list)
{
    
}

static void MP_LoadPlayScreen(void)
{

}

static void MP_LoadFileListScreen(void)
{

}

static void MP_RefreshScreen(void)
{
    
}


