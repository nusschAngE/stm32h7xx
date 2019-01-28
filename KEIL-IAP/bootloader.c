
#include "stm32h7xx.h"

#include "delay.h"
#include "printf.h"

typedef void (*JumpAPP)(void);

/* addr : APP Code address
*/
void load_app(uint32_t addr)
{
    __IO uint32_t spAddr = 0;
    JumpAPP jumpApp = NULL;

    spAddr = *(__IO uint32_t *)addr;//top of main stack
    jumpApp = (JumpAPP)(*(__IO uint32_t *)(addr + 4));
    //printf("addr = 0x%08x, spAddr = 0x%08x, jumpApp = 0x%08x\r\n", addr, spAddr, (uint32_t)jumpApp);
    //TimDelayMs(500);
    if((spAddr&0x2FF00000) == 0x24000000)//valid address?
    {        
        __set_MSP(spAddr);
        jumpApp();//RESET()
    }
    else
    {
        printf("load app error!!\r\n");
        while(1)
        {
        }
    }
}

int main(void)
{
    uint8_t ret = 0;
    
    /* TIM delay us/ms */
    ret = DelayModule_Init();
    if(ret != SYSTEM_ERROR_NONE) {HANDLE_ERROR();}
    /* USART1 printf */
    ret = PrintfModule_Init();
    if(ret != SYSTEM_ERROR_NONE) {HANDLE_ERROR();}

    printf("load app...\r\n");
    TimDelayMs(2000);
    load_app(0x08020000);
    while(1)
    {
        
    }
}












