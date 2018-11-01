
#include <ucos_ii.h>
#include "tp_track.h"
#include "touch.h"

#include "ui_event.h"
#include "ui_handler.h"

#define TOUCH_PRESS_FIX         (5)

#define TOUCH_SLIDE_LIMIT       (20)
//#define TOUCH_SLIDE_UD_FIX      (40)
//#define TOUCH_SLIDE_LR_FIX      (60)

/* TP EVENT */
static TpEvent tpEvt;
static uint16_t holdCnt;
static bool prevTouch = FALSE;


static inline void tp_SendEvent(TpEvent *evt);

/***************** PUBLIC FUNCTION ********************/

void tp_track_Init(void)
{
    tpEvt.flag = 0;
    tpEvt.point = 0;
    tpEvt.gesture = 0;
    holdCnt = 0;
}

void tp_Task(void *p_arg)
{
    uint8_t actPoint = 0;
    signed short xShift = 0, yShift = 0;
    uint16_t xTmp = 0, yTmp = 0;

    bool sendEvt = FALSE;

    while(1)
    {
        if(tpDev.scanFunc)
        {   
            //one point detected only
            actPoint = tpDev.scanFunc(TP_SCAN_POLLING);
        }
        
        sendEvt = FALSE;
        if(actPoint)
        {
            if(actPoint == 1)
            {
                if(prevTouch == FALSE)
                {
                    prevTouch = TRUE;
                    xTmp = tpDev.xPos[0];
                    yTmp = tpDev.yPos[0];
                }

				//printf("tp[%d], %d - %d\r\n", actPoint, tpDev.xPos[0], tpDev.yPos[0]);
			
                tpEvt.xPos = tpDev.xPos[0];
                tpEvt.yPos = tpDev.yPos[0];

                xShift = xTmp - tpEvt.xPos;
                if(xShift < 0)  xShift = 0 - xShift;
                
                yShift = yTmp - tpEvt.yPos;
                if(yShift < 0)  yShift = 0 - yShift;

                if(holdCnt > 64999)
                {
                    holdCnt = 0;
                }
                holdCnt++;

                if((xShift < TOUCH_PRESS_FIX) && (yShift < TOUCH_PRESS_FIX))
                {
                    if((holdCnt >= TP_PRESS_DELAY) && (holdCnt < TP_PRESS2HOLD_DELAY))
                    {
                        if(tpEvt.flag == TP_KEY_NONE)
                        {
                            tpEvt.flag = TP_KEY_DOWN;
                            tpEvt.point = 1;
                            sendEvt = TRUE;
                        }
                    }

                    if(holdCnt >= TP_PRESS2HOLD_DELAY)
                    {
                        if(tpEvt.flag == TP_KEY_DOWN)
                        {
                            tpEvt.flag = TP_KEY_HOLD;
                            tpEvt.point = 1;
                            sendEvt = TRUE;
                        }
                    }

                    if((holdCnt % TP_HOLD2REPEAT_DELAY) == 0)
                    {
                        if((tpEvt.flag == TP_KEY_HOLD) || (tpEvt.flag == TP_KEY_REPEAT))
                        {
                            tpEvt.flag = TP_KEY_REPEAT;
                            tpEvt.point = 1;
                            sendEvt = TRUE;
                        }
                    }
                }//press fix
            }//valid point
        }
        else
        {
            if(actPoint == 0)
            {
                if(prevTouch)
                {
                    if((xShift < TOUCH_PRESS_FIX) && (yShift < TOUCH_PRESS_FIX))
                    {
                        if(tpEvt.flag == TP_KEY_DOWN)
                        {
                            tpEvt.flag = TP_KEY_UP_BEFORE_HOLD;
                            tpEvt.point = 1;
                            sendEvt = TRUE;
                        }

                        if((tpEvt.flag == TP_KEY_HOLD) || (tpEvt.flag == TP_KEY_REPEAT))
                        {
                            tpEvt.flag = TP_KEY_UP_AFTER_HOLD;
                            tpEvt.point = 1;
                            sendEvt = TRUE;
                        }
                    }
                    else
                    {
                        if((tpEvt.flag == TP_KEY_NONE) || (tpEvt.flag == TP_KEY_DOWN))
                        {
                            if((xShift > yShift) && (xShift >= TOUCH_SLIDE_LIMIT))//maybe slide left/right
                            {
                                if(xTmp > tpDev.xPos[0])//left
                                {
                                    tpEvt.gesture = TP_GESTURE_SLIDE_LEFT;
                                    tpEvt.point = 1;
                                    sendEvt = TRUE;
                                }
                                else if(xTmp < tpDev.xPos[0])//right
                                {
                                    tpEvt.gesture = TP_GESTURE_SLIDE_RIGHT;
                                    tpEvt.point = 1;
                                    sendEvt = TRUE;
                                }
                            }
                            else if((xShift < yShift) && (yShift >= TOUCH_SLIDE_LIMIT))//maybe slide up/down
                            {
                                if(yTmp > tpDev.yPos[0])//up
                                {
                                    tpEvt.gesture = TP_GESTURE_SLIDE_UP;
                                    tpEvt.point = 1;
                                    sendEvt = TRUE;
                                }
                                else if(yTmp < tpDev.yPos[0])//down
                                {
                                    tpEvt.gesture = TP_GESTURE_SLIDE_DOWN;
                                    tpEvt.point = 1;
                                    sendEvt = TRUE;
                                }
                            }

                            printf("tpEvt.gesture = %d\r\n", tpEvt.gesture);
                        }
                    }
                }
                /* current release */
				prevTouch = FALSE;
            }			
        }

        if(sendEvt)
        {
            tp_SendEvent(&tpEvt);
        }

		if(actPoint == 0)
		{
			/* clear state */
			tpEvt.flag = 0;
			tpEvt.point = 0;
			tpEvt.gesture = 0;
			holdCnt = 0;
			xTmp = yTmp = 0;
		}

        OSTimeDlyHMSM(0, 0, 0, 10);
    }
}

static inline void tp_SendEvent(TpEvent *evt)
{
    UiSendTpEvent(evt);
}

