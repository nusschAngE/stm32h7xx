#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "my_malloc.h"
#include "public.h"
#include "fatfs_ex.h"

#include "lcd.h"
#include "delay.h"
#include "io_keypad.h"
#include "text.h"
#include "audio_api.h"

#include "K6502.h"
#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_pAPU.h"

/*  STATIC
*/
static uint8_t InfoNES_SwitchWavBuffer(void);

//.nes File Header
NesHeader *Neshd;
NesResource *NES;
ApuResource *APU;
CpuResource *CPU;
ApuEvent *ApuEventQueue;
Mapper1Res *MAP1;
Mapper4Res *MAP4;
Mapper6Res *MAP6;
Mapper21Res *MAP21;
Mapper23Res *MAP23;
Mapper33Res *MAP33;
Mapper118Res *MAP118;
MapperMISRes *MAPMIS;
Mapper1Res MYMAP1;


BYTE *RAM;//8K
BYTE *SRAM;//8K 
BYTE *PPURAM;//16K
BYTE *ChrBuf;//32K
BYTE *ROM;//256k
BYTE *VROM;//256k
BYTE *SRAMBANK;//SRAM BANK(8Kb)
//ROM BANK ( 8Kb*4 ) 
BYTE *ROMBANK0;
BYTE *ROMBANK1;
BYTE *ROMBANK2;
BYTE *ROMBANK3;
//1Kb*16
BYTE *PPUBANK[ 16 ];

WORD *wave_buffers;
WORD *wavBuffer0;
WORD *wavBuffer1;
__IO uint8_t wavXsferCplt;
__IO uint8_t whichBuffer;//which buffer free


bool keyUp = FALSE;
bool keyDown = FALSE;
bool keyLeft = FALSE;
bool keyRight = FALSE;

FIL  NesFile;

//extern void InfoNES_Main(void);

uint8_t InfoNES_BuffersInit(void)
{
    Neshd = myMalloc(MLCSRC_AXISRAM, sizeof(NesHeader));
    NES = myMalloc(MLCSRC_AXISRAM, sizeof(NesResource));
    CPU = myMalloc(MLCSRC_AXISRAM, sizeof(CpuResource));
    RAM = myMalloc(MLCSRC_AXISRAM, RAM_SIZE);
    SRAM = myMalloc(MLCSRC_AXISRAM, SRAM_SIZE);
    PPURAM = myMalloc(MLCSRC_AXISRAM, PPURAM_SIZE);
    ROM = myMalloc(MLCSRC_SDRAM, 256*1024);
    VROM = myMalloc(MLCSRC_SDRAM, 256*1024);
    ChrBuf = myMalloc(MLCSRC_AXISRAM, 256*2*8*8);
    APU = myMalloc(MLCSRC_AXISRAM, sizeof(ApuResource));    
    ApuEventQueue = myMalloc(MLCSRC_AXISRAM, APU_EVENT_MAX*sizeof(ApuEvent));    
    wave_buffers = myMalloc(MLCSRC_AXISRAM, 1470);
    wavBuffer0 = myMalloc(MLCSRC_AXISRAM, 1470);
    wavBuffer1 = myMalloc(MLCSRC_AXISRAM, 1470);   
    
    if(!NES || !RAM || !SRAM || !PPURAM || !APU || !ApuEventQueue
       || !ROM || !VROM || !ChrBuf || !wave_buffers || !wavBuffer0 || !wavBuffer1)
    {
        return 1;
    }

    return 0;
}

#if 0
void NES_Main(void)
{
    Neshd = myMalloc(MLCSRC_AXISRAM, sizeof(NesHeader));
    NES = myMalloc(MLCSRC_AXISRAM, sizeof(NesResource));
    CPU = myMalloc(MLCSRC_AXISRAM, sizeof(CpuResource));
    RAM = myMalloc(MLCSRC_AXISRAM, RAM_SIZE);
    SRAM = myMalloc(MLCSRC_AXISRAM, SRAM_SIZE);
    PPURAM = myMalloc(MLCSRC_AXISRAM, PPURAM_SIZE);
    ROM = myMalloc(MLCSRC_SDRAM, 256*1024);
    VROM = myMalloc(MLCSRC_SDRAM, 256*1024);
    ChrBuf = myMalloc(MLCSRC_AXISRAM, 256*2*8*8);
    APU = myMalloc(MLCSRC_AXISRAM, sizeof(ApuResource));    
    ApuEventQueue = myMalloc(MLCSRC_AXISRAM, APU_EVENT_MAX*sizeof(ApuEvent));    
    wave_buffers = myMalloc(MLCSRC_AXISRAM, 1470);
    Abuf1 = myMalloc(MLCSRC_AXISRAM, 1470);
    Abuf2 = myMalloc(MLCSRC_AXISRAM, 1470);   
    
    if(!NES || !RAM || !SRAM || !PPURAM || !APU || !ApuEventQueue
       || !ROM || !VROM || !ChrBuf || !wave_buffers || !Abuf1 || !Abuf2)
    {
        return;
    }
    
    if(InfoNES_Load((char *)"0:/NES/test.nes") != 0)
    {
		InfoNES_MessageBox("load rom failed!");
        myFree(MLCSRC_AXISRAM, Neshd);
		return;
	}
    /*lcd init*/
    LCD_Clear(COLOR_BLACK);
    LCD_SetWorkRegion(0, 0, NES_DISP_WIDTH, NES_DISP_HEIGHT);
    LCD_StartDataSession();

    /* InfoNES Load */
    InfoNES_Main();	
}
#endif

void InfoNES_Wait(void)
{
}  

int InfoNES_Menu(void)
{
	//0:Normally,-1:Exit InfoNES
  	return 0;
}

void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
	memcpy( dest, src, count );
	return dest;
}

void *InfoNES_MemorySet( void *dest, int c, int count )
{
	memset( dest, c, count);  
	return dest;
}

void InfoNES_SoundInit( void ) 
{
	//Audio_PlayInit();
}

int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ) 
{
#if 0
    AudioDecode_InitTypedef DecInit;

    DecInit.buffer0 = NULL;
    DecInit.buffer1 = NULL;
    DecInit.buffSize = 0;
    DecInit.totalBytes = 0;
    DecInit.byteRate = 0;
    DecInit.protocal = 1;
    DecInit.sampleRate = (uint32_t)sample_rate;
    DecInit.dataSize = 16;
    DecInit.FillTxBuffer = NULL;
    DecInit.SwitchTxBuffer = InfoNES_SwitchWavBuffer;
	Audio_DecodeInit(&DecInit);
	Audio_PlayInit();
	
    wavXsferCplt = 1;
    whichBuffer = 1;
	NES->APU_Mute = 0;
    Audio_SingalBufferPlayStart((uint8_t *)wavBuffer0, 1470);
#endif   
	return 1;
}

void InfoNES_SoundClose( void ) 
{
	Audio_PlayStop();
}

void InfoNES_SoundOutput(int samples, WORD *wave)
{	
 	int i;	

	while(!wavXsferCplt)
	{
	}

    wavXsferCplt=0;
    
    if(whichBuffer == 0)//buffer0 free
    {
        for(i=0; i<735; i++)
            wavBuffer0[i] = wave[i]<<5;
    }
    else  //buffer1 free
    {
        for(i=0; i<735; i++)
            wavBuffer1[i] = wave[i]<<5; 
    }
}

void InfoNES_MessageBox( char *pszMsg, ... )
{
    Font_Typedef font;

    LCD_Clear(COLOR_BLACK);

    font.bg = COLOR_BLACK;
    font.fg = COLOR_WHITE;
    font.size = FONT_SIZE_16;
	ShowTextLineAscii(10,10,(const char*)pszMsg, font);
	
	TimDelayMs(100);
}

void InfoNES_ReleaseRom()
{
#if 0
    myFree(MLCSRC_AXISRAM, Neshd);
    myFree(MLCSRC_AXISRAM, NES);
    myFree(MLCSRC_AXISRAM, CPU);
    myFree(MLCSRC_AXISRAM, RAM);
    myFree(MLCSRC_AXISRAM, SRAM);
    myFree(MLCSRC_AXISRAM, PPURAM);
    myFree(MLCSRC_SDRAM, ROM);
    myFree(MLCSRC_SDRAM, VROM);
    myFree(MLCSRC_AXISRAM, ChrBuf);
    myFree(MLCSRC_AXISRAM, APU);    
    myFree(MLCSRC_AXISRAM, ApuEventQueue);    
    myFree(MLCSRC_AXISRAM, wave_buffers);
    myFree(MLCSRC_AXISRAM, wavBuffer0);
    myFree(MLCSRC_AXISRAM, wavBuffer1); 

    if(MAP1) {myFree(MLCSRC_AXISRAM, MAP1);}
    if(MAP4) {myFree(MLCSRC_AXISRAM, MAP4);}
    if(MAP6) {myFree(MLCSRC_AXISRAM, MAP6);}
    if(MAP21) {myFree(MLCSRC_AXISRAM, MAP21);}
    if(MAP23) {myFree(MLCSRC_AXISRAM, MAP23);}
    if(MAP33) {myFree(MLCSRC_AXISRAM, MAP33);}
    if(MAP118) {myFree(MLCSRC_AXISRAM, MAP118);}
    if(MAPMIS) {myFree(MLCSRC_AXISRAM, MAPMIS);}
#endif
}

void InfoNES_PadState( unsigned long *pdwPad1, unsigned long *pdwPad2, unsigned long *pdwSystem )
{
    static uint8_t Count = 0; 
    uint8_t key = GPIO_KEY_NUM;

	*pdwPad1=0;

	if(++Count == 255)
	{
	    Count = 0;
        key = IoKey_Scan();
    }
	    
	if(key==GPIO_KEY3) *pdwPad1 |= 0x10;//4
    if(key==GPIO_KEY1) *pdwPad1 |= 0x20;//5
    if(key==GPIO_KEY2) *pdwPad1 |= 0x40;//6
    if(key==GPIO_KEY0) *pdwPad1 |= 0x80;//7
	//if(!KEY1)*pdwPad1|=0x04;//2
    //if(!KEY2)*pdwPad1|=0x08;//3
    //if(!KEY3)*pdwPad1|=0x02;//1
    //if(!KEY4)*pdwPad1|=0x01;//0    
}

int InfoNES_ReadRom(const char *pszFileName)
{
    int  size; 
	int  res,nIdx;
	UINT br;

	printf("read ROM[%s]\r\n", pszFileName);
	res = f_open(&NesFile, pszFileName, FA_READ) ;		
	if(res == FR_OK)
	{
		InfoNES_MessageBox("NES File Open OK!"); 
	}
	else
	{
		InfoNES_MessageBox("NES File Open Error!"); 	
		f_close(&NesFile);
		return -1;
	}
	//Read ROM Header
	f_read(&NesFile, Neshd, 16, &br);   
	if((Neshd->byID[0] != 0x4e) || (Neshd->byID[1] != 0x45)
		||(Neshd->byID[2] != 0x53) || (Neshd->byID[3] != 0x1a))
    {
        return -1;  
    }
    //Mapper number
    NES->MapperNo = Neshd->byInfo1>>4;
    /* skip */
    for(nIdx=4; nIdx<8 && Neshd->byReserve[nIdx]==0; ++nIdx );
    /*  */
    if(nIdx == 8)
    {
        NES->MapperNo |= (Neshd->byInfo2&0xf0);//Mapper Number is 8bits 
    }

    switch(NES->MapperNo)
    {
        case 1:   MAP1 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper1Res)); break;
        case 4:   MAP4 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper4Res)); break;
        case 6:   MAP6 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper6Res)); break;
        case 21:  MAP21 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper21Res)); break; 
        case 23:  MAP23 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper23Res)); break; 
        case 33:  MAP33 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper33Res)); break; 
        case 118: MAP118 = myMalloc(MLCSRC_AXISRAM, sizeof(Mapper118Res)); break;      
        case 32:
        case 64:
        case 69:    
            MAPMIS = myMalloc(MLCSRC_AXISRAM, sizeof(MapperMISRes)); break;    
        default: break;    
    }     
  	myMemset(SRAM, 0, SRAM_SIZE);
  	//If trainer presents Read Triner at 0x7000-0x71ff
  	if(Neshd->byInfo1 & 4)
  	{
	 	res = f_read(&NesFile, &SRAM[0x1000], 512, &br);
	 	if(res) 
        {
            printf("Load SRAM error, res = %d\r\n", res);
            while(1);
        }
  	}

  	/* Load ROM data to SDRAM */
  	size = Neshd->byRomSize * 0x4000 ;
	res = f_read(&NesFile, ROM, size, &br);
    if(res) 
    {
        printf("Load ROM error, res = %d\r\n", res);
        while(1);
    }
    
  	if(Neshd->byVRomSize)
  	{
  	    /* Load VROM data to SDRAM */
    	size = Neshd->byVRomSize * 0x2000 ;
		res = f_read(&NesFile, VROM, size, &br);
        if(res) 
        {
            printf("Load VROM error, res = %d\r\n", res);
            while(1);      
        }
  	}
  	f_close(&NesFile);
  	return 0;
}

/**** PRIVATE FUNCTION *****/
static uint8_t InfoNES_SwitchWavBuffer(void)
{
    if(whichBuffer == 1)
    {
        Audio_SingalBufferPlayStart((uint8_t *)wavBuffer1, 1470);
        whichBuffer = 0;
    }
    else
    {
        Audio_SingalBufferPlayStart((uint8_t *)wavBuffer0, 1470);
        whichBuffer = 1;
    }       
    wavXsferCplt = 1;   

    return 0;
}

