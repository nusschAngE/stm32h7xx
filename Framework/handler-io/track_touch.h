
#ifndef _TRACK_TOUCH_H
#define _TRACK_TOUCH_H

#include "public.h"


#define TP_DOWN_DEFALUT_DELAY   (2)
#define TP_HOLD_DEFAULT_DELAY   (20)
#define TP_REPEAT_DEFAULT_DELAY (10)


uint8_t TP_TrackInit(void);
void TP_TrackTask(void *p_arg);

#endif

