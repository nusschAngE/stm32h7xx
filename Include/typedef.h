
#ifndef _TYPEDEF_H
#define _TYPEDEF_H

/* system error, loop */
#define HANDLE_ERROR()              do{} while(1)

#ifndef __IO
#define __IO	volatile
#endif

/************* ARMCC Link *****************/
#define _SRAM12_RW      __attribute__((section(".SRAM12")))
#define _SRAM4_RW       __attribute__((section(".SRAM4")))
#define _ITCM_RW        __attribute__((section(".ITCM")))
#define _DTCM_RW        __attribute__((section(".DTCM")))
#ifdef ExtSDRAM_ENABLE
#define _EXTSDRAM_RW    __attribute__((section(".ExtSDRAM")))
#endif //ExtSDRAM_ENABLE

#ifdef USE_MDK_STDINT
#include <stdint.h>
#else

typedef   signed char   int8_t;
typedef unsigned char   uint8_t;
typedef   signed short  int16_t;
typedef unsigned short  uint16_t;
typedef   signed int    int32_t;
typedef unsigned int    uint32_t;
typedef   signed long   int64_t;
typedef unsigned long   uint64_t;

//typedef unsigned short  DWORD;
//typedef unsigned int    WORD;

#endif //USE_MDK_STDINT

//LCD display
#define LCD_COLOR_FRAME         2U/*RGB565*/
#if (LCD_COLOR_FRAME == 1U)
    typedef unsigned char  LCD_COLOR;
#elif (LCD_COLOR_FRAME == 2U)
    typedef unsigned short  LCD_COLOR;
#else
    typedef unsigned int  LCD_COLOR;
#endif


enum
{
    SYSTEM_ERROR_NONE = 0,
    SYSTEM_ERROR_INIT,
};


/******* typedef ********/
typedef unsigned int  size_t;
typedef unsigned char bool;

/* boolean */
#undef TRUE
#define TRUE	(1)

#undef FALSE
#define FALSE	(0)

#ifndef NULL
#define NULL    (void *)0
#endif

















#endif //_TYPEDEF_H

