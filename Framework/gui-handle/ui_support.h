
#ifndef _UI_SUPPORT_H
#define _UI_SUPPORT_H

#include "public.h"
#include "ui_event.h"

/* app level */
#define MAX_UI_LEVEL        (4u)

#define UI_LEVEL_PLAY       (0)
#define UI_LEVEL_MENU       (1)
#define UI_LEVEL_CONFIRM    (2)
#define UI_LEVEL_SPLASH     (3)

/*  */
typedef uint8_t UiLevel;
typedef bool    (*EventHandler)(UiEvent *uiEvt, void *p_arg);
typedef void    (*UnloadApp)(void);

#endif

