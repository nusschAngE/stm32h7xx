
#include "stm32h7xx.h"
#include "ucos_ii.h"
#include "touch.h"
#include "ui_event.h"
#include "track_touch.h"

/*  STATIC 
*/
static void TpSendEvt(TpEvent_t *evt);

/** private value ********************/
static uint16_t tpXpos[TP_MAX_POINT];
static uint16_t tpYpos[TP_MAX_POINT];

static TpEvent_t  TpEvt;
static uint16_t TpHoldCnt;

static uint8_t PointNbr = 0;

/******** PUBLIC FUNCTION *********/

uint8_t TP_TrackInit(void)
{    
    TpEvt.xPos = 0;
    TpEvt.yPos = 0;
    TpEvt.flag = 0;
    TpEvt.point = 0;
    TpEvt.gesture = 0;

    TpHoldCnt = 0;

    return (0);
}

/*  touch track task body
*   !!curren read one point only
*/
void TP_TrackTask(void *p_arg)
{
    bool sendEvt = FALSE;
    
    while(1)
    {
#if 1
        PointNbr = TP_PointChecked();
        if(PointNbr == 1)
        {
       		/* get point xy */
       		TP_ReadPoint(0, &tpXpos[0], &tpYpos[0]);
       		//printf("point = %d-%d\r\n", tpXpos[0], tpYpos[0]);
         	/* pressing */
         	if(++TpHoldCnt > 64999) {TpHoldCnt = 1;}
         	/* track */
         	if((TpHoldCnt >= TP_DOWN_DEFALUT_DELAY)
              	&&(TpHoldCnt < TP_HOLD_DEFAULT_DELAY))
          	{
               	TpEvt.xPos = tpXpos[0];
              	TpEvt.yPos = tpYpos[0];
              	TpEvt.flag = TP_ACT_DOWN;
               	sendEvt = TRUE;
          	}

         	if(TpHoldCnt >= TP_HOLD_DEFAULT_DELAY)
           	{
               	if(TpEvt.flag == TP_ACT_DOWN)
              	{
                   	TpEvt.xPos = tpXpos[0];
                  	TpEvt.yPos = tpYpos[0];
                  	TpEvt.flag = TP_ACT_HOLD;
                  	sendEvt = TRUE;
              	}
         	}

         	if((TpHoldCnt%TP_REPEAT_DEFAULT_DELAY) == 0)
          	{
               	if((TpEvt.flag == TP_ACT_HOLD)
                  	||((TpEvt.flag == TP_ACT_REPEAT)))
               	{
                  	TpEvt.xPos = tpXpos[0];
                   	TpEvt.yPos = tpYpos[0];
                  	TpEvt.flag = TP_ACT_REPEAT;
                 	sendEvt = TRUE;
              	}
         	}
        }
        else if(PointNbr == 0)
        {
            if(TpEvt.flag == TP_ACT_DOWN)
            {
                TpEvt.xPos = tpXpos[0];
                TpEvt.yPos = tpYpos[0];
                TpEvt.flag = TP_ACT_UP_BEFORE_HOLD;
                sendEvt = TRUE;
            }

            if((TpEvt.flag == TP_ACT_HOLD)
                ||(TpEvt.flag == TP_ACT_REPEAT))
            {
                TpEvt.xPos = tpXpos[0];
                TpEvt.yPos = tpYpos[0];
                TpEvt.flag = TP_ACT_UP_AFTER_HOLD;
                sendEvt = TRUE;
            }

            TpHoldCnt = 0;
            tpXpos[0] = tpYpos[0] = 0;
        }
        else
        {
        	
        }
#endif
        if(sendEvt)
        {
            sendEvt = FALSE;
            TpSendEvt(&TpEvt);
        }
        
        OSTimeDlyHMSM(0, 0, 0, 20);
    }
}

/*** PRIVATE FUNCTION ****/
static void TpSendEvt(TpEvent_t *evt)
{
    UiSendTpEvt(evt);
}

