
#include "stm32h7xx.h"

#include "delay.h"
#include "audio_i2s.h"

static SAI_HandleTypeDef AudioSAI;

/************** PUBLIC FUNCTION *****************/

void HAL_SAI_MspInit(SAI_HandleTypeDef * hsai)
{
    GPIO_InitTypeDef GPIO_Init;

    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_Init.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 \
                    GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Alternate = GPIO_AF2_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_Init);

    hsai->Instance = SAI1;    
}

uint8_t AudioIIS_Init(void)
{
    uint32_t ret = 0;
    
    AudioSAI.Init.AudioMode = SAI_MODEMASTER_TX;
    AudioSAI.Init.Synchro = SAI_ASYNCHRONOUS;
    AudioSAI.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
    AudioSAI.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
    AudioSAI.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    AudioSAI.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;
    AudioSAI.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    AudioSAI.Init.Mckdiv = 0;
    AudioSAI.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
    AudioSAI.Init.MonoStereoMode = SAI_STEREOMODE;
    AudioSAI.Init.CompandingMode = SAI_NOCOMPANDING;
    AudioSAI.Init.TriState = SAI_OUTPUT_NOTRELEASED;
    AudioSAI.Init.Protocol = SAI_FREE_PROTOCOL;
    AudioSAI.Init.DataSize = SAI_DATASIZE_8;
    AudioSAI.Init.FirstBit = SAI_FIRSTBIT_MSB;
    AudioSAI.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
    /* PDM Init */
    AudioSAI.Init.PdmInit.Activation = DISABLE;
    AudioSAI.Init.PdmInit.MicPairsNbr = 1;
    AudioSAI.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;

    ret = HAL_SAI_Init(&AudioSAI);
    if(ret == HAL_OK)
    {
        return (1);
    }

    return (0);
}

/* @param SR : sample rate
*/
uint8_t AudioIIS_SetSampleRate(uint32_t SR)
{
    
    return (1)
}

uint8_t AudioIIS_Transmit(uint8_t *pBuff, uint16_t size)
{
    
}

uint8_t AudioIIS_Receive(uint8_t *pBuff, uint16_t size)
{
    
}







