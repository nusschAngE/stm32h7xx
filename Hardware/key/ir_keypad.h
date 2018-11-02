
#ifndef _IR_KEYPAD_H
#define _IR_KEYPAD_H

#include "public.h"

typedef unsigned char   IRCODE;

/*  func
*/
extern void IRKey_Init(void);
extern IRCODE IRKey_GetCode(void);

#endif


