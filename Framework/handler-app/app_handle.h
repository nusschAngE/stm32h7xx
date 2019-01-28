
#ifndef _APP_HANDLE_H
#define _APP_HANDLE_H

#include "public.h"
#include "ui_event.h"

/* app level */
#define MAX_APP_LEVEL       (3u)

#define APP_LEVEL_PLAY      (0)
#define APP_LEVEL_MENU      (1)
#define APP_LEVEL_SPLASH    (2)

typedef unsigned char AppLevel_t;

/*  */
typedef bool    (*EvtHandler)(UiEvent_t *evt, void *p_arg);
typedef void    (*UnloadApp)(void);

typedef struct
{
    EvtHandler EvtHandler;
    UnloadApp  Unload;
}AppHandle_t;

extern AppHandle_t *AppHandler[MAX_APP_LEVEL];

/*  */
extern uint8_t AppsInit(void);
extern void App_Register(AppHandle_t *app, AppLevel_t level);
extern void App_Deregister(AppHandle_t *app, AppLevel_t level);


extern void app_handler(void *p_arg);

#endif

