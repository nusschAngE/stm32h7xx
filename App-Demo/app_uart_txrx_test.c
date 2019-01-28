
#include "stm32h7xx.h"
#include "app_demo.h"

#include "uart_atk.h"
#include "wifi_esp8266.h"
#include "io_keypad.h"
#include "mytext.h"
#include "delay.h"

#if (MODULE_UART_RXTX_TEST)

#define ATK_UARTBUFFER_SIZE (512U)

static uint8_t ATKUartTxBuffer[ATK_UARTBUFFER_SIZE];
static uint8_t ATKUartRxBuffer[ATK_UARTBUFFER_SIZE];

static __IO uint16_t Idx = 0;
static __IO uint16_t ATKUartRxLen = 0;
static __IO bool IsATKBufferBusy = FALSE;


static void ATKUart_RxParser(uint8_t data)
{
    if(!IsATKBufferBusy)
    {
        ATKUartRxBuffer[Idx] = data;
        printf("%c", ATKUartRxBuffer[Idx]);
        /* over size */
        if(++Idx == ATK_UARTBUFFER_SIZE)
        {
        	printf("rx over size\r\n");
            ATKUartRxLen = Idx;
            IsATKBufferBusy = TRUE;
            Idx = 0;
        }
    }
}

static void ATKUart_RxCpltProc(void)
{
	ATKUartRxLen = Idx;
    IsATKBufferBusy = TRUE;
    Idx = 0;
    printf("atk uart rx cplt, len = %d\r\n", ATKUartRxLen);
}

static void ATKUart_TxCpltProc(void)
{
	printf("atk uart tx cplt\r\n");
}

static void ATKUart_ErrorProc(void)
{
    
}

static UartDevice_t AtkUart = 
{
    UART_MODE_TX_RX,
    115200,
    ATKUart_RxParser,
    ATKUart_RxCpltProc,
    ATKUart_TxCpltProc,
    ATKUart_ErrorProc,
};


void ATKUart_Test(void)
{
    #define TMP_BUFFER_SIZE     (512U)
    
    uint8_t *tmpBuffer = NULL;
    uint8_t key = GPIO_KEY_NUM, Count = 0;

    ShowTextLineAscii(10, 10, "##ATK UART Test##", FontPreset(2));

    ATKUart_Init(&AtkUart);
    
    ShowTextLineAscii(10, 30, "Test Count : ", FontPreset(1));
    ShowTextLineAscii(10, 50, "Uart Receive : ", FontPreset(1));
    
    while(1)
    {
        if(key == GPIO_KEY1)
        {
            sprintf((char *)ATKUartTxBuffer, "Test Count = %d", ++Count);
            ATKUart_TransmitIT(ATKUartTxBuffer, myStrlen((const char *)ATKUartTxBuffer));

            ShowTextLineAscii(114, 30, (const char*)int2str((int64_t)Count), FontPreset(1));

    		myMemset(ATKUartRxBuffer, 0, ATK_UARTBUFFER_SIZE);
            while(ATKUartRxLen == 0) {};//wait
            printf("rx : %s, len = %d\r\n", ATKUartRxBuffer, ATKUartRxLen);
            ShowTextLineAscii(26, 70, (const char*)ATKUartRxBuffer, FontPreset(1));

            ATKUartRxLen = 0;
            IsATKBufferBusy = FALSE;
        }

        TimDelayMs(10);
        key = IoKey_Scan();
        if(key == GPIO_KEY0)
        {
            break;
        }
    }

AtkUart_Test_End:
    ATKUart_DeInit(&AtkUart);
    printf("ATK UART test end.\r\n");
}


#endif

