
#include "stm32h7xx.h"
#include "app_demo.h"
#include "mytext.h"
#include "my_malloc.h"

#if (MODULE_MYMALLOC_TEST)
/* my malloc test
*/
static uint8_t MallocTBuffer[512];

void MyMalloc_Test(void)
{
    uint16_t i = 0;

    uint8_t *tmpAXISRAM = NULL;
    uint8_t *tmpAHBSRAM12 = NULL;
    uint8_t *tmpAHBSRAM4 = NULL;
    uint8_t *tmpDTCM = NULL;
    uint8_t *tmpITCM = NULL;
    uint8_t *tmpSDRAM = NULL;

    ShowTextLineAscii(10, 10, "##Malloc Test##", FontPreset(2));
    
    for(i = 0; i < 512; i++)
        MallocTBuffer[i] = i;//0 ~ 255
#if 1
    tmpAXISRAM = (uint8_t *)myMalloc(MLCSRC_AXISRAM, 512);
    printf("tmpAXISRAM = %p\r\n", tmpAXISRAM);
    if(tmpAXISRAM)
    {
        myMemcpy(tmpAXISRAM, MallocTBuffer, 256);
        if(myMemcmp(tmpAXISRAM, MallocTBuffer, 256) != 0)
        {
            printf("AXISRAM malloc test error\r\n");
        }
        else
        {
            printf("AXISRAM malloc test ok\r\n");
        }
    }
#endif

#if 1
    tmpAHBSRAM12 = (uint8_t *)myMalloc(MLCSRC_AHBSRAM12, 512);
    printf("tmpAHBSRAM12 = %p\r\n", tmpAHBSRAM12);
    if(tmpAHBSRAM12)
    {
        myMemcpy(tmpAHBSRAM12, MallocTBuffer, 256);
        if(myMemcmp(tmpAHBSRAM12, MallocTBuffer, 256) != 0)
        {
            printf("AHBSRAM12 malloc test error\r\n");
        }
        else
        {
            printf("AHBSRAM12 malloc test ok\r\n");
        }
    }
#endif
#if 1
    tmpAHBSRAM4 = (uint8_t *)myMalloc(MLCSRC_AHBSRAM4, 512);
    printf("tmpAHBSRAM4 = %p\r\n", tmpAHBSRAM4);
    if(tmpAHBSRAM4)
    {
        myMemcpy(tmpAHBSRAM4, MallocTBuffer, 256);
        if(myMemcmp(tmpAHBSRAM4, MallocTBuffer, 256) != 0)
        {
            printf("AHBSRAM4 malloc test error\r\n");
        }
        else
        {
            printf("AHBSRAM4 malloc test ok\r\n");
        }
    }
#endif
#if 1
    tmpDTCM = (uint8_t *)myMalloc(MLCSRC_DTCM, 512);
    printf("tmpDTCM = %p\r\n", tmpDTCM);
    if(tmpDTCM)
    {
        myMemcpy(tmpDTCM, MallocTBuffer, 256);
        if(myMemcmp(tmpDTCM, MallocTBuffer, 256) != 0)
        {
            printf("DTCM malloc test error\r\n");
        }
        else
        {
            printf("DTCM malloc test ok\r\n");
        }
    }
#endif    
#if 1
    tmpITCM = (uint8_t *)myMalloc(MLCSRC_ITCM, 512);
    printf("tmpITCM = %p\r\n", tmpITCM);
    if(tmpITCM)
    {
        myMemcpy(tmpITCM, MallocTBuffer, 256);
        if(myMemcmp(tmpITCM, MallocTBuffer, 256) != 0)
        {
            printf("ITCM malloc test error\r\n");
        }
        else
        {
            printf("ITCM malloc test ok\r\n");
        }
    }
#endif
#if defined(ExtSDRAM_ENABLE)
	tmpSDRAM = (uint8_t *)myMalloc(MLCSRC_SDRAM, 512);
    printf("tmpSDRAM = %p\r\n", tmpSDRAM);
    if(tmpSDRAM)
    {
        myMemcpy(tmpSDRAM, MallocTBuffer, 256);
        if(myMemcmp(tmpSDRAM, MallocTBuffer, 256) != 0)
        {
            printf("SDRAM malloc test error\r\n");
        }
        else
        {
            printf("SDRAM malloc test ok\r\n");
        }
    }
#endif //ExtSDRAM_ENABLE
    myFree(MLCSRC_AXISRAM, tmpAXISRAM);
    myFree(MLCSRC_AHBSRAM12, tmpAHBSRAM12);
    myFree(MLCSRC_AHBSRAM4, tmpAHBSRAM4);
    myFree(MLCSRC_DTCM, tmpDTCM);
    myFree(MLCSRC_ITCM, tmpITCM);
    myFree(MLCSRC_ITCM, tmpSDRAM);
    
    printf("myMalloc test end.\r\n");
}

#endif

