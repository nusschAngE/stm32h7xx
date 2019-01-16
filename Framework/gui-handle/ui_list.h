
#ifndef _UI_LIST_H
#define _UI_LIST_H

#include "public.h"
#include "font.h"

typedef struct
{
    uint8_t *title;
    void (*selectFunc)(void *p_arg);
}_ListITEM;

typedef struct
{
    _ListITEM **list;
    uint16_t  listNbr;
}List_Typedef;


#endif //_UI_LIST_H

