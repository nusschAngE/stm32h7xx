
#ifndef _APP_BASE_HOME_H
#define _APP_BASE_HOME_H

#include "public.h"


typedef struct
{
    uint16_t appicon_sl;
    uint16_t appicon_unsl;
    const uint8_t appname[32];
    void (*loadapp)(void *p_arg);
    void *p_arg;
}AppEntry_t;


/*** PUBLIC FUNCTION ****/
void Home_LoadApp(void);

#endif

