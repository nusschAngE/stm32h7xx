
#ifndef _IO_KEYPAD_H
#define _IO_KEYPAD_H


#include "public.h"

typedef unsigned int    KEY_SCAN_VAL; 

/*  GPIO KEY
*/
enum
{
    GPIO_KEY0 = 0,
    GPIO_KEY1,
    GPIO_KEY2,
    GPIO_KEY3,

    GPIO_KEY_NUM,
};




/*  FUNC
*/
extern void ioKey_Init(void);
extern KEY_SCAN_VAL ioKey_GetKey(void);




#endif
