
#ifndef _TOUCH_H
#define _TOUCH_H

#include "public.h"

#define TOUCH_DRIVER_IC_GT9147  1U

/*  TP SCAN TYPE
*/
#define TP_SCAN_POLLING     0
#define TP_SCAN_TRIGGER     1

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

    uint8_t direct;
    uint8_t actPoint;
    
    uint8_t key;
    uint8_t gesture;
    
    uint8_t (*scanFunc)(uint8_t mode);
}TP_Device;

/* TP DEVICE */
extern TP_Device tpDev;

extern uint8_t TP_Init(void);
extern bool TP_ReadInterrupt(void);
extern uint8_t TP_PointChecked(void);
extern uint8_t TP_ReadPoint(uint8_t point, uint16_t *xPos, uint16_t *yPos);
#endif

