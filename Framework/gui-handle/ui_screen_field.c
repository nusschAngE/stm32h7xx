
#include "ui_screen_field.h"








bool UiMatchTpPoint(ScreenRegion_Typedef region, uint16_t tpXpos, uint16_t tpYpos)
{
    uint8_t match = FALSE;

    if((tpXpos >= region.x) && (tpXpos < region.x+region.w)) {
        if((tpYpos >= region.y) && (tpYpos < region.y+region.h)) {
            match = TRUE;
        }
    }
    return (match);
}





