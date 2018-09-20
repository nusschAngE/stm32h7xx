
#ifndef _KEYPAD_H
#define _KEYPAD_H

#include "public.h"

typedef enum
{
    KEY_PRESS,
    KEY_RELEASE,
}keyStatus_Enum;

typedef enum
{
    KEY_TYPE_IDLE = 0,
    KEY_TYPE_DOWN,
    KEY_TYPE_UP_BEFORE_HOLD,
    KEY_TYPE_HOLD,
    KEY_TYPE_REPEAT,
    KEY_TYPE_UP_AFTER_HOLD,
    KEY_TYPE_UP,

    KEY_TYPE_NUM
}keyType_Enum;

typedef enum
{
    KEY_0 = 0,
    KEY_1,
    KEY_2,
    KEY_3,

    KEY_NUM
}key_Enum;

/* key code */
volatile extern uint32_t keyCode;


void keypad_init(void);
uint8_t keypad_read(uint8_t key);

#endif


