
#include "stm32h7xx.h"
#include "my_malloc.h"

#if 0

/* base address */
__ALIGNED(64) uint8_t MEMSRAM_BASE[MEMSRAM_SIZE];
__ALIGNED(64) uint8_t MEMSDRAM_BASE[MEMSDRAM_SIZE] __attribute__((section(".ARM.__at_0xC0000000")));
//__ALIGNED(64) uint8_t MEMDTCM_BASE[MEMDTCM_SIZE] __attribute__((section(".ARM.__at_0x20000000")));
__ALIGNED(64) uint8_t MEMDTCM_BASE[MEMDTCM_SIZE] __attribute__((section("IRAM1")));

/* manage map */
uint32_t MEMSRAM_MAP[MEMSRAM_BLOCKS];
uint32_t MEMSDRAM_MAP[MEMSDRAM_BLOCKS] __attribute__((section(".ARM.__at_0xC1400000")));
//uint32_t MEMDTCM_MAP[MEMDTCM_BLOCKS] __attribute__((section(".ARM.__at_0x2000F000"))); 
uint32_t MEMDTCM_MAP[MEMDTCM_BLOCKS] __attribute__((section("IRAM1")));

const uint32_t memBlocks[MALLOC_SRC_NUM] = {MEMSRAM_BLOCKS, MEMSDRAM_BLOCKS, MEMDTCM_BLOCKS};
const uint32_t memBLKSize[MALLOC_SRC_NUM] = {MEMSRAM_BLKSIZE, MEMSDRAM_BLKSIZE, MEMDTCM_BLKSIZE};
const uint32_t memSize[MALLOC_SRC_NUM] = {MEMSRAM_SIZE, MEMSDRAM_SIZE, MEMDTCM_SIZE};

/* private */
static void Malloc_Init(uint8_t mlcSrc);
static uint16_t Malloc_GetUsed(uint8_t mlcSrc);

static MallocDev mallocDev = {
    Malloc_Init,
    Malloc_GetUsed,
    {MEMSRAM_BASE, MEMSDRAM_BASE, MEMDTCM_BASE},
    {MEMSRAM_MAP, MEMSDRAM_MAP, MEMDTCM_MAP},
    {FALSE, FALSE, FALSE}
};

static void Malloc_Init(uint8_t mlcSrc)
{
    myMemset(mallocDev.memMap[mlcSrc], 0, memBlocks[mlcSrc]);
    mallocDev.memRdy[mlcSrc] = TRUE;
}

static uint16_t Malloc_GetUsed(uint8_t mlcSrc)
{
    uint32_t used = 0, i;

    for(i = 0; i < memBlocks[mlcSrc]; i++)
    {
        if(mallocDev.memMap[mlcSrc][i])
        {
            used++;
        }
    }

    return (uint16_t)((used * 1000)/memBlocks[mlcSrc]);
}

/******************** PUBLIC FUNCTION **********************/


void *myMalloc(uint8_t mlcSrc, uint32_t size)
{
    uint32_t idx, i;
    uint32_t BlocksFind, BlocksNeed; 
    void *ret = NULL;

    /* size==0 */
    if(!size)
    {
        return (NULL);
    }

    if(mallocDev.memRdy[mlcSrc] == FALSE)
    {
	    mallocDev.init(mlcSrc);
    }
		
    BlocksNeed = size / memBLKSize[mlcSrc];
    if(size % memBLKSize[mlcSrc]) 
    {
        BlocksNeed++;
    }

    for(idx = memBlocks[mlcSrc] - 1; idx >= 0; idx--)
    {
        if(mallocDev.memMap[mlcSrc][idx] == 0)//block free
        {
            BlocksFind++;
        }
        else //not continuous
        {
            BlocksFind = 0;
        }
        
        if(BlocksFind == BlocksNeed)
        {
            for(i = 0; i < BlocksNeed; i++)
            {
                mallocDev.memMap[mlcSrc][idx + i] = BlocksNeed;//mark it as used
            }
            /* update ret */
            ret = (void *)(mallocDev.memBase[mlcSrc] + (idx * memBLKSize[mlcSrc]));

            break;
        }
    }

    return (ret);
}

void myFree(uint8_t mlcSrc, void *src)
{
    int64_t ofs;
    uint32_t idx, BlocksFree, i;

    ofs = (uint32_t)src - (uint32_t)mallocDev.memBase[mlcSrc];

    if((ofs) && (ofs < memSize[mlcSrc]))
    {        
        idx = ofs / memBLKSize[mlcSrc];
        BlocksFree = mallocDev.memMap[mlcSrc][idx];

        for(i = 0; i < BlocksFree; i++)
        {
            mallocDev.memMap[mlcSrc][idx + i] = 0;//block free
        }
    }
}

void *myMemcpy(void *dst, void *src, uint32_t size)
{
    uint8_t *d = dst;
    uint8_t *s = src;

    while(size--)
    {
        *d++ = *s++;
    }

    return (dst);
}

void *myMemset(void *src, uint8_t ch, uint32_t size)
{
    uint8_t *s = src;

    while(size--)
    {
        *s++ = ch;
    }

    return (src);
}

void *myRealloc(uint8_t mlcSrc, void *src, uint32_t size)
{
    void *tmp = NULL;

    if((size == 0) || (src == NULL))
    {
        return (NULL);
    }

    if(mallocDev.memRdy[mlcSrc] == FALSE)
    {
	    mallocDev.init(mlcSrc);
    }

    tmp = myMalloc(mlcSrc, size);
    if(tmp != NULL)
    {
        myMemcpy(tmp, src, size);
        myFree(mlcSrc, src);
    }

    return (tmp);
}

#endif

