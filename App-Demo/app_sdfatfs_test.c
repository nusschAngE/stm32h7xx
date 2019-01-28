
#include "stm32h7xx.h"
#include "app_demo.h"
#include "delay.h"
#include "fatfs_ex.h"
#include "mytext.h"

#if (MODULE_SDFATFS_TEST)
/* Fat FS test function
*/
void SDFatFS_Test(void)
{  
    FRESULT fret = FR_OK;
    
    FATFS *tFS = NULL;
    FATFS **tmpFS = NULL;
    FILINFO *tFInfo = NULL;
    FIL *tFile = NULL;
    DIR *tDir = NULL;
    uint8_t *tmpBuffer = NULL;
    DWORD FsFree = 0;
    UINT fileRead = 0, Count = 10;

    ShowTextLineAscii(10, 10, "##SD FATFS Test##", FontPreset(2));

    tFS = (FATFS *)myMalloc(MLCSRC_AXISRAM, sizeof(FATFS));
    if(tFS == NULL)
    {
        printf("FATFS malloc error\r\n");
        goto FatFS_Test_End;
    }

    tFInfo = (FILINFO *)myMalloc(MLCSRC_AXISRAM, sizeof(FILINFO));
    if(tFInfo == NULL)
    {
        printf("FILINFO malloc error\r\n");
        goto FatFS_Test_End;
    }

    tFile = (FIL *)myMalloc(MLCSRC_AXISRAM, sizeof(FIL));
    if(tFile == NULL)
    {
        printf("FIL malloc error\r\n");
        goto FatFS_Test_End;
    }

    tDir = (DIR *)myMalloc(MLCSRC_AXISRAM, sizeof(DIR));
    if(tDir == NULL)
    {
        printf("DIR malloc error\r\n");
        goto FatFS_Test_End;
    }

    tmpBuffer = (uint8_t *)myMalloc(MLCSRC_AXISRAM, 512);
    if(tmpBuffer == NULL)
    {
        printf("tmpBuffer malloc error\r\n");
        goto FatFS_Test_End;
    }

    fret = f_mount(tFS, (const TCHAR*)"0:", 0);
    if(fret != FR_OK)
    {
        printf("f_mount() error, fret = %d\r\n", fret);
        goto FatFS_Test_End;
    }

    fret = f_getfree((const TCHAR*)"0:", &FsFree, tmpFS);
    if(fret != FR_OK)
    {
        printf("f_getfree() error, fret = %d\r\n", fret);
        goto FatFS_Test_End;
    }
    else
    {
        printf("fs free = %ld\r\n", FsFree);
    }

    fret = f_findfirst(tDir, tFInfo, (const TCHAR*)"0:", (const TCHAR *)".txt");
    if(fret != FR_OK)
    {
        printf("f_findfirst() error, fret = %d\r\n", fret);
        goto FatFS_Test_End;
    }
    else
    {
        printf("file name = ");
        //printf(tFInfo->fname);
        printf("\r\n");
        printf("file altname = %s\r\n", tFInfo->altname);
    }

    while(Count--)
    {   
        fret = f_open(tFile, (const TCHAR *)tFInfo->altname, FA_READ|FA_OPEN_EXISTING);
        if(fret != FR_OK)
        {
            printf("open exist file error, fret = %d\r\n", fret);
            break;
        }

        fret = f_read(tFile, tmpBuffer, 512, &fileRead);
        if(fret != FR_OK)
        {
            printf("read exist file error, fret = %d\r\n", fret);
            break;
        }

        fret = f_close(tFile);
        if(fret != FR_OK)
        {
            printf("close exist file error, fret = %d\r\n", fret);
        }

        fret = f_open(tFile, (const TCHAR *)"count.txt", FA_READ|FA_WRITE|FA_OPEN_APPEND);
        if(fret == FR_OK)
        {
        	int ret = 0;
            sprintf((char*)tmpBuffer, "%d, ", Count);
            ret = f_puts((const TCHAR *)tmpBuffer, tFile);
            if(ret == EOF)
            {
                printf("f_puts() error, fret = %d\r\n", ret);
                break;
            }
            fret = f_close(tFile);
            if(fret != FR_OK)
            {
                printf("close file 'count.txt' error, fret = %d\r\n", fret);
                break;
            }
        }
        else
        {
            printf("open file 'count.txt' error, fret = %d\r\n", fret);
            break;
        }
    }

FatFS_Test_End: 

    myFree(MLCSRC_AXISRAM, tFS);
    myFree(MLCSRC_AXISRAM, tFInfo);
    myFree(MLCSRC_AXISRAM, tFile);
    myFree(MLCSRC_AXISRAM, tDir);
    myFree(MLCSRC_AXISRAM, tmpBuffer);

    printf("SD FatFS test end.\r\n");
}
#endif

