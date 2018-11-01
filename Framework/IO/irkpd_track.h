
#ifndef _IRKEY_TRACK_H
#define _IRKEY_TRACK_H


#include "public.h"
#include "ir_keypad.h"


/* key map */
typedef struct
{
    IRCODE code;
    uint8_t key;
    uint8_t holdDelay;
    uint8_t repeatDelay;
}IRKey_Map;


extern void irkpd_track_Init(void);
extern void irkpd_Task(void *p_arg);



#endif


