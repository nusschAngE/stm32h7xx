

#ifndef _IOKPD_TRACK_H
#define _IOKPD_TRACK_H


#include "public.h"


/* key map */
typedef struct
{
    uint8_t kpd;
    uint8_t key;
    uint8_t holdDelay;
    uint8_t repeatDelay;
}IOKey_Map;


/* kpd function */
extern void iokpd_track_Init(void);
extern void iokpd_Task(void *p_arg);



#endif

