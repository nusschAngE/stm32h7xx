
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
//typedef uint8_t(*tp_init)(void);
//typedef uint8_t(*tp_scan)(uint8_t);


enum
{
    TP_STA_OK = 0,
    TP_STA_TIMEOUT,
    TP_STA_ERRIC,
    
    TP_STA_ERROR
};

enum
{
    TP_KEY_NONE = 0,
    TP_KEY_DOWN = 1,
    TP_KEY_UP_BEFORE_HOLD,
    TP_KEY_HOLD,
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

/************** TP DEFINES ******************/
#define TP_MAX_POINT    (5)

#define TP_POINT1       (0x01)
#define TP_POINT2       (0x02)
#define TP_POINT3       (0x04)
#define TP_POINT4       (0x08)
#define TP_POINT5       (0x10)

/* max size */
#define TOUCH_HW_W      (480)
#define TOUCH_HW_H      (800)

typedef struct
{
    uint16_t xPos[TP_MAX_POINT];
    uint16_t yPos[TP_MAX_POINT];

    uint8_t  direct;
    uint8_t  actPoint;
    
    uint8_t key;
    uint8_t gesture;

    uint8_t (*scanFunc)(uint8_t);
}_tp_dev;

typedef struct
{
    uint16_t xPos[TP_MAX_POINT];
    uint16_t yPos[TP_MAX_POINT];
    uint8_t key[TP_MAX_POINT];

    uint8_t gesture;
}_tp_event;

/* TP DEVICE */
extern _tp_dev tpDev;
extern _tp_event tpEvent;

extern uint8_t GT9147_Init(void);
extern uint8_t GT9147_Scan(uint8_t mode);

extern void touch_Init(void);
extern void touchScan_Task(void);
#endif

