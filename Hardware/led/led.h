
#ifndef _LED_H
#define _LED_H

#include "public.h"

#define LED_NONE    0
#define LED_GREEN   1
#define LED_RED     2

/****************** GLOBAL FUNCTION *****************/
extern void LED_Init(void);
extern void LED_Toggle(uint8_t led);
extern void LED_Onoff(uint8_t led, bool on);

#endif

