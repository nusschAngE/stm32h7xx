

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/******* boolean ********/
typedef unsigned char bool;

#undef TRUE
#define TRUE	(1)

#undef FALSE
#define FALSE	(0)


/* system defines */
//LCD display
#define LCD_DRV_NT35510         1U/*MPU interface*/
#define LCD_COLOR_FRAME         2U/*565*/
#if (LCD_COLOR_FRAME == 1U)
    typedef unsigned char  LCD_COLOR;
#elif (LCD_COLOR_FRAME == 2U)
    typedef unsigned short  LCD_COLOR;
#else
    typedef unsigned int  LCD_COLOR;
#endif


















#endif
