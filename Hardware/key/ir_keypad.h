
#ifndef _IR_KEYPAD_H
#define _IR_KEYPAD_H

#include "public.h"

typedef unsigned char   IRCODE;

/*  func
*/
extern void irKey_Init(void);
extern IRCODE irKey_GetCode(void);

#endif


