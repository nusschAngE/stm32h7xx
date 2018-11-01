

#ifndef _TP_TRACK_H
#define _TP_TRACK_H

#include "public.h"

#define TP_PRESS_DELAY          (2)
#define TP_PRESS2HOLD_DELAY     (90)
#define TP_HOLD2REPEAT_DELAY    (15)

enum
{
    TP_KEY_NONE = 0,
    TP_KEY_DOWN = 1,//
    TP_KEY_UP_BEFORE_HOLD,
    TP_KEY_HOLD,
    TP_KEY_REPEAT,
    TP_KEY_UP_AFTER_HOLD,
};

enum
{   
    TP_GESTURE_NONE = 0,
    TP_GESTURE_SLIDE_UP = 1,
    TP_GESTURE_SLIDE_DOWN,
    TP_GESTURE_SLIDE_LEFT,
    TP_GESTURE_SLIDE_RIGHT,
    TP_GESTURE_ZOOM_UP,
    TP_GESTURE_ZOOM_DOWN,
};




extern void tp_track_Init(void);
extern void tp_Task(void *p_arg);


#endif


