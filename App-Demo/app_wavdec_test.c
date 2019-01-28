
#include "stm32h7xx.h"
#include "app_demo.h"

#include "delay.h"
#include "fatfs_ex.h"
#include "mytext.h"
#include "audio_api.h"
#include "wav_codec.h"
#include "led.h"
#include "io_keypad.h"
#include "usb_host.h"
#include "usbh_diskio.h"

#if (MODULE_WAVDECODE_TEST)

//#define FATFS_IN_USB           
//#define FS_TEST             USBFatFS
//#define FS_DRV              "1:"
//#define FILE_DIR            "1:/MUSIC"

#define FS_TEST             SDFatFS
#define FS_DRV              "0:"
#define FILE_DIR            "0:/MUSIC"

#define AUD_BUFF_SIZE       (8192U)
#define FIL_BUFF_SIZE       (8192U)
#define PROC_BUFF_SIZE      (1024U)
#define PATH_BUFF_SIZE      (1024U)

typedef struct
{
    TCHAR *path[128];
    uint8_t listNbr;
}FileList_t;

FIL AudioFile;
FILINFO AudioFilInfo;
DIR AudioDir;
bool fileEnd = FALSE;
WavHeader_t WavHeader;
FileList_t FiltList;
AudioDecInit_t AudDecInit;

uint8_t *AudBuffer0 = NULL;
uint8_t *AudBuffer1 = NULL;
uint8_t *FileBuff = NULL;
uint8_t *ProcBuff = NULL;
TCHAR *PathBuff = NULL;

//AudioDecDrv_t AudioDrv;

static uint8_t WavReadData(uint8_t *buff, uint32_t size, uint8_t dataSize)
{
    FRESULT fret = FR_OK;
    uint32_t needSize = 0, bRead = 0, relSize = 0, bFilled = 0;
	//printf("read, %d-", dataSize);
    if(fileEnd)
    {
        myMemset(buff, 0, size);
        return (FILL_BUFF_OK);
    }
    
    switch (dataSize)
    {          
        case 32:
        case 16:
            {
                relSize = size;
                needSize = size;
                fret = f_read(&AudioFile, buff, needSize, &bRead);
                if(fret == FR_OK)
                {           
                    if(bRead < needSize) 
                    {
                        relSize -= bRead;
                        bFilled += bRead;
                        myMemset(buff+bFilled, 0, relSize);
                        fileEnd = TRUE;
                        return (FILL_BUFF_END);
                    }
                }
                else
                {
                	printf("f_read() ret = %d\r\n", fret);
                    return (FILL_BUFF_ERR);
                }
            }
            break;
        case 24:
            {
                uint32_t *tmpPtr = (uint32_t *)buff;
                uint32_t i,j;
                
                relSize = size;
                needSize = size*3/4;
                fret = f_read(&AudioFile, FileBuff, needSize, &bRead);
                if(fret == FR_OK)
                {        
                    for(i=j=0; i < bRead; i+=3, j++)
                    {
                       //tmpPtr[j] = (*(uint32_t *)(FileBuff+i)) & 0x00ffffff;
                       tmpPtr[j] = (*(uint32_t *)(FileBuff+i)) << 8;
                    }
                    
                    if(bRead < needSize) 
                    {
                        relSize -= j*4;
                        bFilled += j*4;
                        myMemset(buff+bFilled, 0, relSize);
                        fileEnd = TRUE;
                        return (FILL_BUFF_END);
                    }
                }
                else
                {
                	printf("f_read() ret = %d\r\n", fret);
                    return (FILL_BUFF_ERR);
                }
            }
            break;
        case 8:
            {
                uint16_t *tmpPtr = (uint16_t *)buff;
                uint32_t i,j;
                
                relSize = size;
                needSize = size/2;
                fret = f_read(&AudioFile, FileBuff, needSize, &bRead);
                if(fret == FR_OK)
                {
                    for(i=j=0; i < bRead; i++, j++)
                    {
                        tmpPtr[j] = (uint16_t)FileBuff[i] << 8;
                        //tmpPtr[j] = (uint16_t)FileBuff[i];
                    }
                    
                    if(bRead < needSize) 
                    {
                        relSize -= j*2;
                        bFilled += j*2;
                        myMemset(buff+bFilled, 0, relSize);
                        fileEnd = TRUE;
                        return (FILL_BUFF_END);
                    }
                }
                else
                {
                	printf("f_read() ret = %d\r\n", fret);
                    return (FILL_BUFF_ERR);
                }
            }
            break;
        default:
        	printf("error data size\r\n");
            return (FILL_BUFF_ERR);
        
    }
    
    return (FILL_BUFF_OK);
}

void WavDecode_Test(void)
{
    uint8_t ret = 0;
    /* play info */
    uint32_t bytesSend = 0, totalBytes = 0;
    uint32_t totalSecond = 0, curSecond = 0, codecTime = 0;
    /* display */
    uint16_t x = 10, y = 30;
    FONT_t font;
    /* key */
    uint32_t ledCnt = 0, keyPress = 0xff;
    /* file */
    uint8_t listNbr = 0, listIdx = 0, FilType = T_UNSUPRT;
    UINT fileRead = 0;
    uint32_t bDecode = 0;

    printf("/*------------- WAV Decode Test --------------------*/\r\n");

    ShowTextLineAscii(8, 10, "### WAV Decoder ###", FontPreset(1));

    AudBuffer0 = (uint8_t*)myMalloc(MLCSRC_SDRAM, AUD_BUFF_SIZE);
    AudBuffer1 = (uint8_t*)myMalloc(MLCSRC_SDRAM, AUD_BUFF_SIZE);
    FileBuff = (uint8_t*)myMalloc(MLCSRC_AXISRAM, FIL_BUFF_SIZE);
    ProcBuff = (uint8_t*)myMalloc(MLCSRC_AXISRAM, PROC_BUFF_SIZE);
    PathBuff = (TCHAR*)myMalloc(MLCSRC_AXISRAM, PATH_BUFF_SIZE);
    if(!AudBuffer0 || !AudBuffer0 || !AudBuffer1 || !FileBuff || !ProcBuff || !PathBuff)
    {
        printf("buffer malloc error\r\n");
        goto WavDecodeTest_End;
    }
    printf("AudBuffer0 = %p, AudBuffer1 = %p\r\n", AudBuffer0, AudBuffer1);
#if 0    
    /* audio driver init */
    ret = AudioDrv_Register(&AudioDrv);
    if(ret != 0)
    {
        printf("audio driver register error, ret = %d\r\n", ret);
        goto WavDecodeTest_End;
    }
#endif

    printf("audio driver init!!\r\n");
    ret += Audio_Init();
    ret += Audio_SetMute(TRUE);
    ret += Audio_SetVolume(35);
    ret += Audio_SetChannel(CODEC_OUTPUT_SPK);
    if(ret != 0)
    {
        printf("audio driver init error, ret = %d\r\n", ret);
        goto WavDecodeTest_End;
    }
    //while(1);
    /* FATFS Init */
    printf("fatfs mount!!\r\n");
#ifdef FATFS_IN_USB    
    MX_USB_HOST_Init();
    while(Appli_state != 2)//wait user active
    {
    	MX_USB_HOST_Process();
    }
#endif //FATFS_IN_USB
    ret = f_mount(&FS_TEST, (const TCHAR*)FS_DRV, 1);
    if(ret != FR_OK)
    {
        printf("f_mount(), ret = %d\r\n", ret);
        goto WavDecodeTest_End;
    }

    ret = f_opendir(&AudioDir, (const TCHAR*)FILE_DIR);
    if(ret != FR_OK)
    {
        printf("f_opendir() error, fret = %d\r\n", ret);
        goto WavDecodeTest_End;
    }

    /* updata list */
    printf("reading file\r\n");
    listIdx = 0;
    for(; listIdx < 128; listIdx++)
    {
        ret = f_readdir(&AudioDir, &AudioFilInfo);
        if(ret == FR_OK)
        {
            uint16_t pathlen = 0;
            TCHAR *path = NULL;
            
            FilType = f_TypeTell(AudioFilInfo.fname);
            if(FilType == T_WAV)
            {
                pathlen = f_GetFilePath(PathBuff, PATH_BUFF_SIZE, (TCHAR*)FILE_DIR, AudioFilInfo.fname);
                path = (TCHAR*)myMalloc(MLCSRC_AXISRAM, pathlen+1);//'\0'
                myMemset(path, 0, pathlen+1);
                myMemcpy(path, (const uint8_t *)PathBuff, pathlen);
                if(path)
                {
                    FiltList.path[listIdx] = path;
                    FiltList.listNbr++;
                }
            }
            else
            {
                //MemPrintf("list name", tFInfo->fname, 64);
            }
        }
    }
    /* close dir */
    f_closedir(&AudioDir);

    /* list display */
    listIdx = 0;
    for(listIdx = 0, x = 8, y = 200; listIdx < FiltList.listNbr; listIdx++)
    {
        ShowTextLineGbk(x, y, (const uint8_t*)FiltList.path[listIdx], GetFontPreset(1));
        y += 20;
    }

    printf("audio playing!!\r\n");
    listIdx = 0;
    font.size = FONT_SIZE_16;
    font.bg = FONT_BG_BLACK;
    font.fg = 0x4f4f;
    ShowTextLineAscii(8, 30, "File: ", font);    
    ShowTextLineAscii(8, 50, "+bits:", font);
    ShowTextLineAscii(8, 70, "+freq:", font);
    ShowTextLineAscii(8, 90, "+kpbs:", font);
    ShowTextLineAscii(8,110, "+time:", font);

    font.size = FONT_SIZE_16;
    font.bg = FONT_BG_BLACK;
    font.fg = 0x1f1f;
    while(1)
    {
    	printf("playing idx = %d\r\n", listIdx);
        myMemset(PathBuff, 0, PROC_BUFF_SIZE);
        f_GetFileName(FiltList.path[listIdx], PATH_BUFF_SIZE, PathBuff);
        ShowTextLineGbk(56, 30, (const uint8_t*)PathBuff, font);

        /* open file */
        ret = f_open(&AudioFile, (const TCHAR*)FiltList.path[listIdx], FA_READ | FA_OPEN_EXISTING);
        if(ret != FR_OK)
        {
            printf("f_open() error, fret = %d\r\n", ret);
            goto WavDecodeTest_End;
        }

        myMemset(ProcBuff, 0, PROC_BUFF_SIZE);
        ret = f_read(&AudioFile, ProcBuff, 512, &fileRead);
        if(ret != FR_OK)
        {
            printf("f_read() error, fret = %d\r\n", ret);
            goto WavDecodeTest_End;
        }

        printf("get wav header info!!\r\n");
        Wav_CodecInit();
        ret = Wav_GetHeader(&WavHeader, ProcBuff, 512, &bDecode);
        if(ret == WAV_DECODE_ERROR_NONE)
        {
            /* bits */
            sprintf((char*)ProcBuff, "%d", WavHeader.Fmt.BitsPerSample);
            ShowTextLineAscii(56, 50, (const char*)ProcBuff, font);
            /* sample rate */
            sprintf((char*)ProcBuff, "%d", WavHeader.Fmt.SampleRate);
            ShowTextLineAscii(56, 70, (const char*)ProcBuff, font);
            /* kbps */
            sprintf((char*)ProcBuff, "%d", WavHeader.Fmt.ByteRate*8/1000);
            ShowTextLineAscii(56, 90, (const char*)ProcBuff, font);
            /* time */
            totalSecond = WavHeader.AudioDataSize / WavHeader.Fmt.ByteRate;
            curSecond = codecTime = 0;
            sprintf((char*)ProcBuff, "00:00 - %02d:%02d", totalSecond/60, totalSecond%60);
            ShowTextLineAscii(56, 110, (const char*)ProcBuff, font);

            /* file seek */
            ret = f_lseek(&AudioFile, bDecode);
            if(ret != FR_OK)
            {
                printf("f_lseek() error, offet = %d, fret = %d\r\n", bDecode, ret);
                goto WavDecodeTest_End;
            }
        }
        else
        {
            printf("get wav header info error, decRet = %d\r\n", ret);
            goto WavDecodeTest_End;
        }

        printf("decode init !!\r\n");
        AudDecInit.sampleRate = WavHeader.Fmt.SampleRate;
        AudDecInit.dataSize = WavHeader.Fmt.BitsPerSample;
        AudDecInit.protocal = 2;
        AudDecInit.totalBytes = WavHeader.AudioDataSize;
        AudDecInit.byteRate = WavHeader.Fmt.ByteRate;
        
        if(WavHeader.Fmt.BitsPerSample == 8)
        {
            AudDecInit.sampleRate = WavHeader.Fmt.SampleRate;
            AudDecInit.totalBytes = WavHeader.AudioDataSize * 2;
            AudDecInit.byteRate = WavHeader.Fmt.ByteRate * 2;
        }

        if(WavHeader.Fmt.BitsPerSample == 24)
        {
            AudDecInit.sampleRate = WavHeader.Fmt.SampleRate;
            AudDecInit.totalBytes = WavHeader.AudioDataSize*4/3;
            AudDecInit.byteRate = WavHeader.Fmt.ByteRate*4/3;
        }

        AudDecInit.buffer0 = AudBuffer0;
        AudDecInit.buffer1 = AudBuffer1;
        AudDecInit.buffSize = AUD_BUFF_SIZE;
        AudDecInit.FillTxBuffer = WavReadData;
        
        ret = Audio_DecodeInit(&AudDecInit);
        if(ret != 0) 
        {
            printf("decode init error, ret = %d\r\n", ret);
          	if(++listIdx >= FiltList.listNbr) 
          	{
          		goto WavDecodeTest_End;
          	}

            continue;
        }    

        /* fill buffer */
        ret = WavReadData(AudDecInit.buffer0, AudDecInit.buffSize, AudDecInit.dataSize);
        ret = WavReadData(AudDecInit.buffer1, AudDecInit.buffSize, AudDecInit.dataSize);
        if(ret != 0)
        {
            printf("fill buffer0 error, ret = %d\r\n", ret);
            goto WavDecodeTest_End;
        }
        
        Audio_SetMute(FALSE);
        Audio_PlayInit();
        Audio_DoubleBufferPlayStart();
        fileEnd = FALSE;
        printf("play start!!\r\n");
        while(1)
        {
            if(Audio_GetState() == AUDIO_STATE_STREAM_END)
            {
            	printf("data stream end!!\r\n");
                if(++listIdx >= FiltList.listNbr) {listIdx = 0;}

                Audio_PlayStop();
                TimDelayMs(200);
                //f_close(&AudioFile);
                break;
            }
            else if(Audio_GetState() == AUDIO_STATE_ERROR)
			{
				printf("audio play error, errCode = %d\r\n", Audio_GetError());
	            if(Audio_GetError() != AUDIO_ERROR_NONE)
	            {
	                if(++listIdx >= FiltList.listNbr) {listIdx = 0;}
	                
	                Audio_PlayStop();
	                TimDelayMs(1000);
	                //f_close(&AudioFile);
	                break;
	            }
            }

			codecTime = Audio_GetCodecTime();
			if(curSecond != codecTime)
			{
				curSecond = codecTime;
				sprintf((char*)ProcBuff, "%02d:%02d", curSecond/60, curSecond%60);
            	ShowTextLineAscii(56, 110, (const char*)ProcBuff, font);
			}

            keyPress = IoKey_Scan();
            if(keyPress == GPIO_KEY0)//next
            {
                printf("go next file!!\r\n");
                if(++listIdx >= FiltList.listNbr) {listIdx = 0;}
                
                Audio_PlayStop();
                TimDelayMs(200);
                //f_close(&AudioFile);
                break;
            }
            else if(keyPress == GPIO_KEY3)//stop
            {
                printf("audio play stop!!\r\n");
                Audio_PlayStop();
                TimDelayMs(200);
                //f_close(&AudioFile);
                goto WavDecodeTest_End;
            }
        }//file play loop
    }//list loop

    /* end function */
WavDecodeTest_End:

    myFree(MLCSRC_AXISRAM, AudBuffer0);
    myFree(MLCSRC_AXISRAM, AudBuffer1);
    myFree(MLCSRC_AXISRAM, FileBuff);
    myFree(MLCSRC_AXISRAM, ProcBuff);
    myFree(MLCSRC_AXISRAM, PathBuff);

    printf("/*------------- WAV Decode Test End --------------*/\r\n");
}

#endif


