
#ifndef _KPD_TRACK_H
#define _KPD_TRACK_H

#include "public.h"


/* key map */
typedef struct
{
    uint8_t kpd;
    uint8_t key;
    uint8_t holdDelay;
    uint8_t repeatDelay;
}IOKeyMap_Typedef;

typedef struct
{
    uint8_t code;
    uint8_t key;
    uint8_t holdDelay;
    uint8_t repeatDelay;
}IRKeyMap_Typedef;

#define KEY_TRACK_TICK_PERIOD_MS    (10)//10MS
/* key track ticks */
#define KEY_DOWN_DEFAULT_DELAY      (5)
#define KEY_HOLD_DEFAULT_DELAY      (100)
#define KEY_REPEAT_DEFAULT_DELAY    (20)


extern uint8_t kpd_TrackInit(void);
extern void kpd_TrackTask(void *p_arg);


#endif


