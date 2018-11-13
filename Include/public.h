

#ifndef _PUBLIC_H
#define _PUBLIC_H

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
typedef unsigned int  size_t;
typedef unsigned char bool;


/* system error, loop */
#define SYS_ERROR()     do{} while(1)

/******** uC/OS ***********/
#define RTOS_uCOS_II    1U

#undef TRUE
#define TRUE	(1)

#undef FALSE
#define FALSE	(0)

#ifndef NULL
#define NULL    (void *)0
#endif

/**************************************/
extern int pkgStrcmp(const char *str1, const char *str2);
extern void *pkgMemset(void *s, int ch, size_t n);
extern char *pkgStrcpy(char *dest, const char *src);
extern char *pkgStrcat(char *dest, const char *src);











#endif

