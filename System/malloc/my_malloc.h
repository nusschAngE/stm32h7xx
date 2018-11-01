
#ifndef _MY_MALLOC_H
#define _MY_MALLOC_H

#include "public.h"

/* MALLOC BANK */
enum
{   
    MALLOC_SRC_SRAM = 0,//internal 
    MALLOC_SRC_SDRAM,   //external
    MALLOC_SRC_DTCM,    //internal ,CPU only

    MALLOC_SRC_NUM
};

/* SRAM malloc info */
#define MEMSRAM_BLKSIZE         (64)
#define MEMSRAM_SIZE            ((uint32_t)(160 * 1024))//160Kbytes
#define MEMSRAM_BLOCKS          (MEMSRAM_SIZE / MEMSRAM_BLKSIZE)

/* SDRAM malloc info */
#define MEMSDRAM_BLKSIZE        (64)//memory block size : 64bytes
#define MEMSDRAM_SIZE           ((uint32_t)(20 * 1024 * 1024))//20Mbytes
#define MEMSDRAM_BLOCKS         (MEMSDRAM_SIZE / MEMSDRAM_BLKSIZE)

/* DTCM malloc info */
#define MEMDTCM_BLKSIZE         (64)
#define MEMDTCM_SIZE            ((uint32_t)(60 * 1024))//60Kbytes
#define MEMDTCM_BLOCKS          (MEMDTCM_SIZE / MEMDTCM_SIZE)


/* Malloc manage */
typedef struct
{
    void (*init)(uint8_t mlcSrc);
    uint16_t (*used)(uint8_t mlcSrc);
    uint8_t *memBase[MALLOC_SRC_NUM];
    uint32_t *memMap[MALLOC_SRC_NUM];
    bool memRdy[MALLOC_SRC_NUM];
}MallocDev;


/******************** PUBLIC FUNCTION **********************/
extern void *myMalloc(uint8_t mlcSrc, uint32_t size);
extern void myFree(uint8_t mlcSrc, void *src);
extern void *myMemcpy(void *dst, void *src, uint32_t size);
extern void *myMemset(void *src, uint8_t ch, uint32_t size);
extern void *myRealloc(uint8_t mlcSrc, void *src, uint32_t size);


#endif

