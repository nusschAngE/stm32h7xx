
#ifndef _BASE_HOME_H
#define _BASE_HOME_H

#include "public.h"


typedef struct
{
    uint16_t appicon_sl;
    uint16_t appicon_unsl;
    const uint8_t appname[16];
    void (*loadapp)(void *p_arg);
}AppEntry_Typedef;


/*** PUBLIC FUNCTION ****/
void Home_LoadApp(void);

#endif

