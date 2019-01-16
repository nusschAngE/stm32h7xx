

#include "stm32h7xx.h"
#include "app_demo.h"

#include "delay.h"
#include "qspi_flash.h"
#include "fatfs_ex.h"
#include "text.h"
#include "font.h"
#include "font_update.h"

#if (MODULE_HZKFONT_TEST)
/* hzk font test
*/
void HZKFont_Test(void)
{
    uint8_t ret = 0;
    uint16_t i = 0;

    FATFS *tFS = NULL;
    FATFS **tmpFs;
    FILINFO *tFInfo = NULL;
    FIL *tFile = NULL;
    DIR *tDir = NULL;
    uint8_t *tmpBuffer = NULL;
    DWORD FsFree = 0;
		UINT bFileRead = 0, bFileWrite = 0;
    uint8_t *namePtr = NULL;

    printf("-------- hzk font test start! --------------\r\n");

    tFS = (FATFS *)myMalloc(MLCSRC_AXISRAM, sizeof(FATFS));
    if(tFS == NULL)
    {
        printf("FATFS malloc error\r\n");
        goto HZKFont_Test_End;
    }

    tFInfo = (FILINFO *)myMalloc(MLCSRC_AXISRAM, sizeof(FILINFO));
    if(tFInfo == NULL)
    {
        printf("FILINFO malloc error\r\n");
        goto HZKFont_Test_End;
    }

    tFile = (FIL *)myMalloc(MLCSRC_AXISRAM, sizeof(FIL));
    if(tFile == NULL)
    {
        printf("FIL malloc error\r\n");
        goto HZKFont_Test_End;
    }

    tDir = (DIR *)myMalloc(MLCSRC_AXISRAM, sizeof(DIR));
    if(tDir == NULL)
    {
        printf("DIR malloc error\r\n");
        goto HZKFont_Test_End;
    }

    tmpBuffer = (uint8_t *)myMalloc(MLCSRC_AXISRAM, 512);
    if(tmpBuffer == NULL)
    {
        printf("tmpBuffer malloc error\r\n");
        goto HZKFont_Test_End;
    }
    
    ret = f_mount(tFS, "0:", 1);
    if(ret != FR_OK)
    {
        printf("f_mount() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }

    ret = f_getfree("0:", &FsFree, tmpFs);
    if(ret != FR_OK)
    {
        printf("f_getfree() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }
    else
    {
        printf("disc [0:] free = %ld\r\n", FsFree);
    }

    /* check hzk font */
    HZKFont_Check();
    if(myMemcmp(HzkFontInfo.Font, "hzk ", 4) != 0)
    {//need update
        ret = HZKFont_Update((const uint8_t*)"0:");
        if(ret != 0)
        {
            printf("HZKFont_Update() error, ret = %d\r\n", ret);
            goto HZKFont_Test_End;
        }
    }
    else
    {
    	printf("hzk font find\r\n");
    }

    ret = f_findfirst(tDir, tFInfo, "0:", "*.txt");
    if(ret != FR_OK)
    {
        printf("f_findfirst() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }

    printf("find file, altname : %s\r\n", tFInfo->altname);
    //MemPrintf("tFInfo->fname", tFInfo->fname, 64);
    ShowTextLineGbk(10, 40, (const uint8_t*)tFInfo->fname, FontPreset(2));

    ret = f_open(tFile, tFInfo->altname, FA_READ|FA_OPEN_EXISTING);
    if(ret != FR_OK)
    {
        printf("f_open() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }
	
    ret = f_read(tFile, tmpBuffer, 512, &bFileRead);
    if(ret != FR_OK)
    {
        printf("f_read() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }

    printf("read file : %d\r\n", bFileRead);
    for(i = 0; i < 256; i++)
    {
        printf("%02x ", tmpBuffer[i]);
    }
    printf("\r\n");
    ShowTextLineGbk(10, 70, tmpBuffer, FontPreset(3));

#if 0
    ret = f_write(tFile, tmpBuffer, bFileRead, &bFileWrite);
    if(ret != FR_OK)
    {
        printf("f_write() error, ret = %d\r\n", ret);
        goto HZKFont_Test_End;
    }
    else
    {
        f_write(tFile, "\r\n", 2, &bFileWrite);
        f_close(tFile);
        f_unmount("0:");
    }
#endif    
    
HZKFont_Test_End:
    f_close(tFile);
    f_unmount("0:");
    
    myFree(MLCSRC_AXISRAM, tFS);
    myFree(MLCSRC_AXISRAM, tFInfo);
    myFree(MLCSRC_AXISRAM, tFile);
    myFree(MLCSRC_AXISRAM, tDir);
    myFree(MLCSRC_AXISRAM, tmpBuffer);

    printf("-------- hzk font test end! --------------\r\n");
}
#endif //MODULE_HZKFONT_TEST

