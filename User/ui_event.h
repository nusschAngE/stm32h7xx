
#ifndef _UI_EVENT_H
#define _UI_EVENT_H

#include "public.h"

#define UI_EVENT_GROUP_Msk          ((uint32_t)0xFF000000)
#define UI_EVENT_GROUP_SYSTEM       ((uint32_t)0x01000000)
#define UI_EVENT_GROUP_KEY          ((uint32_t)0x02000000)
#define UI_EVENT_GROUP_TP           ((uint32_t)0x03000000)

#define UI_EVENT_FLAG_Msk           ((uint32_t)0x00FF0000)
/* key flag */
#define UI_KEY_DOWN                 ((uint32_t)0x00010000)/* key press down */
#define UI_KEY_UPBH                 ((uint32_t)0x00020000)/* key release before hold */
#define UI_KEY_HOLD                 ((uint32_t)0x00030000)/* key hold */
#define UI_KEY_REPEAT               ((uint32_t)0x00040000)/* key hold to repeat */
#define UI_KEY_UPAH                 ((uint32_t)0x00050000)/* key release after hold */
#define UI_KEY_UP                   ((uint32_t)0x00060000)/* key release */

#define UI_EVENT_CODE_Msk           ((uint32_t)0x0000FFFF)


typedef struct
{
    uint32_t code;
    void *value;
}_uiEvent;

extern volatile uint32_t uiEventCode;
extern _uiEvent UIEvent;



#endif

