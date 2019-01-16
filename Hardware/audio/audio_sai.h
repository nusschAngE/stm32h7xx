
#ifndef _AUDIO_SAI_H
#define _AUDIO_SAI_H

#include "public.h"

/* instance */
#define AUDIO_SAI               SAI1_Block_A
#define AUDIO_SAI_CORE_CLOCK    RCC_SAI1CLKSOURCE_PLL2
/* SAI DMA */
#define AUDIO_DMA               DMA1_Stream5
#define AUDIO_DMAIRQn           DMA1_Stream5_IRQn  

#define CSB_FALLINGEDGE         (0)
#define CSB_RISINGEDGE          (1)

/* SAI Error Code */
#define AUDIO_SAIERROR_NONE     (0)
#define AUDIO_SAIERROR_PARAM    (1)
#define AUDIO_SAIERROR_CLK      (2)
#define AUDIO_SAIERROR_INIT     (3)
#define AUDIO_SAIERROR_DMAINIT  (4)
#define AUDIO_SAIERROR_DMASTART (6)
#define AUDIO_SAIERROR_BUFFER   (7)
#define AUDIO_SAIERROR_TX       (8)
#define AUDIO_SAIERROR_RX       (9)
#define AUDIO_SAIERROR_STOP     (10)

#define FREE_BUFFER0            (0x01)
#define FREE_BUFFER1            (0x02)

#define FILL_BUFF_OK            (0)
#define FILL_BUFF_END           (1)
#define FILL_BUFF_ERR           (2)

/* SAI Buffer */
typedef uint8_t(*SAIFillTxBufferFunc)(uint8_t *buff, uint32_t size, uint8_t dataSize);
typedef uint8_t(*SAISwitchTxBufferFunc)(void);

typedef struct
{
    uint8_t *buffer0;
    uint8_t *buffer1;
    uint32_t buffSize;

	uint8_t  rdataSize;
    uint32_t tdataSize;
    uint32_t frequency;
    uint32_t clkStrobing;
    SAIFillTxBufferFunc FillTxBuffer;//called when use double buffer
    SAISwitchTxBufferFunc SwitchTxBuffer;//called when use singal buffer
}SAI_TxInitTypedef;

/* Public Function */
extern uint8_t Audio_SAILowLevelInit(void);
extern uint8_t Audio_SAILowLevelDeInit(void);
extern uint8_t Audio_SAITxConfig(SAI_TxInitTypedef *Init);
extern uint8_t Audio_SAIDataXsfer(uint8_t *pData, uint32_t pSize);
extern uint8_t Audio_SAIMultiBufferTxStart(void);
extern uint8_t Audio_SAITxStop(void);

#endif


