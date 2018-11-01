

#ifndef _PUBLIC_H
#define _PUBLIC_H

<<<<<<< HEAD
#define USE_MDK_STDINT

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

#endif

/******* typedef ********/
=======
#include <stdint.h>

/******** uC/OS ***********/
#define RTOS_uCOS_II    1U

/******* boolean ********/
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
typedef unsigned int  size_t;
typedef unsigned char bool;


<<<<<<< HEAD
/******** uC/OS ***********/
#define RTOS_uCOS_II    1U

=======
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
#undef TRUE
#define TRUE	(1)

#undef FALSE
#define FALSE	(0)

<<<<<<< HEAD
#ifndef NULL
#define NULL    (void *)0
#endif

/**************************************/
extern int pkgStrcmp(const char *str1, const char *str2);
extern void *pkgMemset(void *s, int ch, size_t n);
extern char *pkgStrcpy(char *dest, const char *src);
extern char *pkgStrcat(char *dest, const char *src);
=======

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


/**************************************/
extern int myStrcmp(const char *str1, const char *str2);
extern void *myMemset(void *s, int ch, size_t n);
extern char *myStrcpy(char* dest, const char *src);
extern char *myStrcat(char *dest, const char *src);
>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56











#endif

