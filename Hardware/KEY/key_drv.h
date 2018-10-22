
#ifndef _KEY_DRV_H_
#define _KEY_DRV_H_

#include "public.h"


enum
{
    KEY_0 = 0,
    KEY_1,
    KEY_2,
    KEY_3,

    KEY_NUM
};

enum
{
    KEY_RELEASE = 0,
    KEY_PRESS,
};




/******************* PUBLIC FUNCTION *******************/
extern void keyDrv_Init(void);





#endif

