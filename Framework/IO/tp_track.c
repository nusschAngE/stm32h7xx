
#include <ucos_ii.h>
#include "tp_track.h"
#include "touch.h"

#include "ui_event.h"
#include "ui_handler.h"

#define TOUCH_PRESS_FIX         (3)

#define TOUCH_SLIDE_LIMIT       (20)
//#define TOUCH_SLIDE_UD_FIX      (40)
//#define TOUCH_SLIDE_LR_FIX      (60)

/* TP EVENT */
static TpEvent tpEvt[2];//0 : press, 1 : gesture
static uint16_t holdCnt;
static bool prevTouch = FALSE;


static inline void tp_SendEvent(TpEvent *evt);

/***************** PUBLIC FUNCTION ********************/

void tp_track_Init(void)
{
    tpEvt[0].flag = 0;
    tpEvt[0].point = 0;
    tpEvt[0].gesture = 0;
	 
	tpEvt[1].flag = 0;
    tpEvt[1].point = 0;
    tpEvt[1].gesture = 0;
    
		holdCnt = 0;
}

void tp_Task(void *p_arg)
{
    uint8_t actPoint = 0;
    signed short xShift = 0, yShift = 0;
    uint16_t xTmp = 0, yTmp = 0;

    while(1)
    {
        if(tpDev.scanFunc)
        {   
            //one point detected only
            actPoint = tpDev.scanFunc(TP_SCAN_POLLING);
        }
        
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
			
                tpEvt[0].xPos = tpEvt[1].xPos = tpDev.xPos[0];
                tpEvt[0].yPos = tpEvt[1].yPos = tpDev.yPos[0];

                xShift = xTmp - tpEvt[0].xPos;
                if(xShift < 0)  xShift = 0 - xShift;
                
                yShift = yTmp - tpEvt[0].yPos;
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
                        if(tpEvt[0].flag == TP_KEY_NONE)
                        {
                            tpEvt[0].flag = TP_KEY_DOWN;
                            tpEvt[0].point = 1;
                            tp_SendEvent(&tpEvt[0]);
                        }
                    }

                    if(holdCnt >= TP_PRESS2HOLD_DELAY)
                    {
                        if(tpEvt[0].flag == TP_KEY_DOWN)
                        {
                            tpEvt[0].flag = TP_KEY_HOLD;
                            tpEvt[0].point = 1;
                            tp_SendEvent(&tpEvt[0]);
                        }
                    }

                    if((holdCnt % TP_HOLD2REPEAT_DELAY) == 0)
                    {
                        if((tpEvt[0].flag == TP_KEY_HOLD) || (tpEvt[0].flag == TP_KEY_REPEAT))
                        {
                            tpEvt[0].flag = TP_KEY_REPEAT;
                            tpEvt[0].point = 1;
                            tp_SendEvent(&tpEvt[0]);
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
                        if(tpEvt[0].flag == TP_KEY_DOWN)
                        {
                            tpEvt[0].flag = TP_KEY_UP_BEFORE_HOLD;
                            tpEvt[0].point = 1;
                            tp_SendEvent(&tpEvt[0]);
                        }

                        if((tpEvt[0].flag == TP_KEY_HOLD) || (tpEvt[0].flag == TP_KEY_REPEAT))
                        {
                            tpEvt[0].flag = TP_KEY_UP_AFTER_HOLD;
                            tpEvt[0].point = 1;
                            tp_SendEvent(&tpEvt[0]);
                        }
                    }
                    else
                    {
                        //if((tpEvt[0].flag == TP_KEY_NONE) || (tpEvt[0].flag == TP_KEY_DOWN))
                        {
                            if((xShift > yShift) && (xShift >= TOUCH_SLIDE_LIMIT))//maybe slide left/right
                            {
                                if(xTmp > tpDev.xPos[0])//left
                                {
                                    tpEvt[1].gesture = TP_GESTURE_SLIDE_LEFT;
                                    tpEvt[1].point = 1;
                                    tp_SendEvent(&tpEvt[1]);
                                }
                                else if(xTmp < tpDev.xPos[0])//right
                                {
                                    tpEvt[1].gesture = TP_GESTURE_SLIDE_RIGHT;
                                    tpEvt[1].point = 1;
                                    tp_SendEvent(&tpEvt[1]);
                                }
                            }
                            else if((xShift < yShift) && (yShift >= TOUCH_SLIDE_LIMIT))//maybe slide up/down
                            {
                                if(yTmp > tpDev.yPos[0])//up
                                {
                                    tpEvt[1].gesture = TP_GESTURE_SLIDE_UP;
                                    tpEvt[1].point = 1;
                                    tp_SendEvent(&tpEvt[1]);
                                }
                                else if(yTmp < tpDev.yPos[0])//down
                                {
                                    tpEvt[1].gesture = TP_GESTURE_SLIDE_DOWN;
                                    tpEvt[1].point = 1;
                                    tp_SendEvent(&tpEvt[1]);
                                }
                            }

                            printf("tpEvt.gesture = %d\r\n", tpEvt[1].gesture);
                        }
                    }
                }
                /* current release */
				prevTouch = FALSE;
            }			
        }

		if(actPoint == 0)
		{
			/* clear state */
			tpEvt[0].flag = 0;
			tpEvt[0].point = 0;
			tpEvt[0].gesture = 0;
			
			tpEvt[1].flag = 0;
      		tpEvt[1].point = 0;
      		tpEvt[1].gesture = 0;
			
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

