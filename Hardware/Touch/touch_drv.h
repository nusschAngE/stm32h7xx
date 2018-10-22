
#ifndef _TOUCH_DRV_H_
#define _TOUCH_DRV_H_

#include "public.h"


/*********** touch IIC function ***********/
enum
{
    T_IIC_OK = 0,
    T_IIC_TIMEOUT,

	T_IIC_ERROR
};

extern void T_IIC_Init(void);
extern void T_IIC_Start(void);
extern void T_IIC_Stop(void);
extern uint8_t T_IIC_WaitACK(void);
extern void T_IIC_SendACK(void);
extern void T_IIC_SendNACK(void);
extern void T_IIC_SendByte(uint8_t sVal);
extern uint8_t T_IIC_ReadByte(void);


/************************* GLOBAL FUNCTION *******************************/
void touch_Init(void);





#endif

