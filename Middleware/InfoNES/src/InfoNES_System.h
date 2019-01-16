/*===================================================================*/
/*                                                                   */
/*  InfoNES_System.h : The function which depends on a system        */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_SYSTEM_H_INCLUDED
#define InfoNES_SYSTEM_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"


/* Palette data */
static const WORD NesPalette[ 64 ] =
{
	0x738e,0x20d1,0x0015,0x4013,0x880e,0xa802,0xa000,0x7840,
	0x4140,0x0200,0x0280,0x01c2,0x19cb,0x0000,0x0000,0x0000,
	0xbdd7,0x039d,0x21dd,0x801e,0xb817,0xe00b,0xd940,0xca41,
	0x8b80,0x0480,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
	0xffdf,0x3ddf,0x5c9f,0x445f,0xf3df,0xfb96,0xfb8c,0xfcc7,
	0xf5c7,0x8682,0x4ec9,0x5fd3,0x075b,0x0000,0x0000,0x0000,
	0xffdf,0xaf1f,0xc69f,0xd65f,0xfe1f,0xfe1b,0xfdd6,0xfed5,
	0xff14,0xe7d4,0xaf97,0xb7d9,0x9fde,0x0000,0x0000,0x0000,
};
/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/
//ROM BANK cache
typedef struct 
{
    BYTE buf[8*1024];//8KB 
    WORD page;//page number
}BANKBUF;


void NES_Main(void);
/* Menu screen */
int InfoNES_Menu(void);

/* Read ROM image file */
int InfoNES_ReadRom( const char *pszFileName );

/* Release a memory for ROM */
void InfoNES_ReleaseRom(void);

/* Transfer the contents of work frame on the screen */
void InfoNES_LoadFrame(void);

/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem );

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count );

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count );

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg );

/* Wait */
void InfoNES_Wait(void);

/* Sound Initialize */
void InfoNES_SoundInit( void );

/* Sound Open */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate );

/* Sound Close */
void InfoNES_SoundClose( void );

void GAME_TX_callback(void);
/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples,WORD* wave);

/* Print system message */
void InfoNES_MessageBox( char *pszMsg, ... );

void My_task(void);

#endif /* !InfoNES_SYSTEM_H_INCLUDED */
