
#include <stdarg.h>

#include "stm32h7xx.h"
#include "wifi_esp8266.h"
#include "uart_atk.h"

#include "my_malloc.h"
#include "delay.h"

/********* DEFINES ********************/
#define ESP_REST_PORT		GPIOI
#define ESP_REST_PIN		GPIO_PIN_11
#define ESP_REST_ON()		HAL_GPIO_WritePin(ESP_REST_PORT, ESP_REST_PIN, GPIO_PIN_RESET)
#define ESP_REST_OFF()		HAL_GPIO_WritePin(ESP_REST_PORT, ESP_REST_PIN, GPIO_PIN_SET)


/* pass through packet size */
#define ESP_PACKET_SIZE     (2048U)
#define ESP_CMDBUFF_SIZE    (512U)
#define ESP_DATABUFF_SIZE   (2048U + 32U)

/*  STATIC
*/
static void esp_IoInit(void);
static void esp_SendData(uint8_t *pData, uint16_t size);
static void esp_SendATCmd(const char *atcmd, ...);
static uint8_t esp_WaitResponse(uint8_t *param, uint16_t pSize, const char *response, uint32_t timeout);
static void esp_RxParser(uint8_t data);
static void esp_RxCpltProc(void);
static void esp_TxCpltProc(void);
static void esp_ErrorProc(void);

/********* VALUE ***********************/
static UartDevice_t EspUartDev = 
{
    UART_MODE_TX_RX,
    115200,
    esp_RxParser,
    esp_RxCpltProc,
    esp_TxCpltProc,
    esp_ErrorProc,
};

static uint8_t EspCmdBuffer[ESP_CMDBUFF_SIZE];
static uint8_t EspDataBuffer[ESP_DATABUFF_SIZE];
static __IO uint16_t EspRxDataSize = 0;
static __IO uint16_t EspRxByteIdx = 0;
static __IO bool IsBufferBusy = FALSE;
static __IO bool IsEspRxCplt = FALSE;
static __IO bool IsEspTxCplt = FALSE;

/* IP Connect manage */
//static IPConnect_t ipConnected[4];

/********* PUBLIC FUNCTION ***************/
uint8_t ESP8266_Init(void)
{   
    uint8_t ret = 0;
    /* IO init */
    esp_IoInit();
    /* hard reset */
	ESP_REST_ON();
	TimDelayMs(200);
	ESP_REST_OFF();
	TimDelayMs(1000);//skip boot message
    /* uart init */
    ret = ATKUart_Init(&EspUartDev);
    if(ret != ESP_RES_OK) {return (1);}  
    /* AT Test */
    ret = ESP8266_TestATCmd();
    if(ret != ESP_RES_OK) {return (2);}
    /* tuneoff echo */
    ret = ESP8266_EchoOnoff(0);
    if(ret != ESP_RES_OK) {return (4);}

    /* value init */
    myMemset(EspCmdBuffer, 0, ESP_CMDBUFF_SIZE);
    myMemset(EspDataBuffer, 0, ESP_DATABUFF_SIZE);
    EspRxDataSize = 0;
    EspRxByteIdx = 0;
    IsBufferBusy = FALSE;
    IsEspRxCplt = FALSE;
    IsEspTxCplt = FALSE;

    return (ESP_RES_OK);
}

uint8_t ESP8266_UartConfig(uint8_t baudRate, uint8_t bits, uint8_t stopbits, uint8_t pority, uint8_t flowc)
{
    /* module uart config */

    /* mcu uart config */
	
    return (0);
}

uint8_t ESP8266_TestATCmd(void)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_TEST);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);    
}

uint8_t ESP8266_EchoOnoff(uint8_t on)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_ATE, on);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);
}

uint8_t ESP8266_Reset(void)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_RESET);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);
}

uint8_t ESP8266_SetWlanMode(uint8_t mode)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCWMODE, mode);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);    
}

uint8_t ESP8266_JoinAP(uint8_t *ssid, uint8_t *pwd)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCWJAP, ssid, pwd);
    ret = esp_WaitResponse(NULL, 0, "DISCONNECT", 0x2ffff);
    ret = esp_WaitResponse(NULL, 0, "CONNECTED", 0x2ffff);
    if(ret != ESP_RES_OK)
    {
        printf("join ap error : %s\r\n", ssid);
    }

    return (ret); 
}

/* pBuff[64] */
uint8_t ESP8266_GetSSID(uint8_t *ssid, uint32_t pSize)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_CCWJAP);
    ret = esp_WaitResponse(ssid, pSize, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);    
}

uint8_t ESP8266_QuitAP(void)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_PCWQAP);
    ret = esp_WaitResponse(NULL, 0, "OK", 0x2ffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret); 
}

uint8_t ESP8266_SearchAP(APInfo_t **apInfo, uint8_t *listNbr, uint8_t maxNbr)
{
    return (0);
}

uint8_t ESP8266_ClearAPList(APInfo_t **apInfo, uint8_t listNbr)
{
    return (0);
}

/* client or server */
uint8_t ESP8266_SetIPMode(uint8_t mode)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCIPMODE, mode);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);    
}

uint8_t ESP8266_GetConnectedIP(IPConnect_t **ip)
{
    return (0);
}

uint8_t ESP8266_GetIPConnectState(uint8_t *state)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_PCIPSTATUS);
    ret = esp_WaitResponse(NULL, 0, "ERROR", 0xffff);
    if(ret == ESP_RES_OK) {*state = 0;}//not connected
    else if(ret == ESP_RES_ERROR) {*state = 1;}//connected
    else {}//error
    
    return (ret); 
}

/* single/multiple connect */
uint8_t ESP8266_SetIPConnectMode(uint8_t mode)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCIPMUX, mode);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);     
}

/* single connect 
*   @param type : "TCP"/"UDP"
*   @param ip   : "192.168.2.2"
*   @param port : 
*/
uint8_t ESP8266_IPConnect0(const char *type, uint8_t *ip, uint16_t port)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCIPSTART0, type, ip, port);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);     
}

/* multiple connect
*   @param id   : 0~4
*   @param type : "TCP"/"UDP"
*   @param ip   : "192.168.2.2"
*   @param port : 0~65535
*/
uint8_t ESP8266_IPConnect1(uint8_t id, const char *type, uint8_t *ip, uint16_t port)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_SCIPSTART1, id, type, ip, port);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK)
    {
        //error;
    }

    return (ret);     
}

/* CMD_SCIPSEND0 */
uint8_t ESP8266_IPSend0(uint8_t *pData, uint16_t size, uint32_t timeout)
{
    uint8_t ret = 0;
    uint8_t neverTimeout = (timeout > 0) ? 0 : 1;

    /* over size? on data? */
    if((size > ESP_PACKET_SIZE) || (!pData)) {return (ESP_RES_ERROR);}
    /* send cmd */
    esp_SendATCmd(CMD_SCIPSEND0, size);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK) {}//error
    /* send data */
    esp_SendData(pData, size);
    /* wait send compeleted */
    ret = ESP_RES_TIMEOUT;
    while((neverTimeout) || (timeout--))
    {
        if(IsEspTxCplt)
        {
            ret = ESP_RES_OK;
            break;
        }
    }
    return (ret);    
}

/* CMD_SCIPSEND0 */
uint8_t ESP8266_IPSend1(uint8_t id, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    uint8_t ret = 0;
    uint8_t neverTimeout = (timeout > 0) ? 0 : 1;

    /* over size? on data? */
    if((size > ESP_PACKET_SIZE) || (!pData)) {return (ESP_RES_ERROR);}
    /* send cmd */
    esp_SendATCmd(CMD_SCIPSEND1, id, size);
    ret = esp_WaitResponse(NULL, 0, "OK", 0xffff);
    if(ret != ESP_RES_OK) {}//error
    /* send data */
    esp_SendData(pData, size);
    /* wait send compeleted */
    ret = ESP_RES_TIMEOUT;
    while((neverTimeout) || (timeout--))
    {
        if(IsEspTxCplt)
        {
            ret = ESP_RES_OK;
            break;
        }
    }
    return (ret);    
}

uint8_t ESP8266_StartPassThrough(uint16_t size)
{
    uint8_t ret = 0;

    esp_SendATCmd(CMD_PCIPSEND);
    ret = esp_WaitResponse(NULL, 0, ">", 0xffff);
    if(ret != ESP_RES_OK) {}//error    

    return (ret);
}

/* one packet, 2048KBytes max */
uint8_t ESP8266_PassThrough0(uint8_t *pData, uint16_t size, uint32_t timeout)
{
    uint8_t ret = 0;
    uint8_t neverTimeout = (timeout > 0) ? 0 : 1;

    /* over size? on data? */
    if((size > ESP_PACKET_SIZE) || (!pData)) {return (ESP_RES_ERROR);}
    /* send cmd */
    esp_SendData(pData, size);
    /* wait send compeleted */
    ret = ESP_RES_TIMEOUT;
    while((neverTimeout) || (timeout--))
    {
        if(IsEspTxCplt)
        {
            ret = ESP_RES_OK;
            break;
        }
    }
    return (ret);  
}

/* multiple packets, 2048KBytes/packet */
uint8_t ESP8266_PassThrough1(uint8_t *pData, uint16_t size, uint32_t timeout)
{
    uint8_t neverTimeout = (timeout > 0) ? 0 : 1;
    uint32_t sendSize = 0, relSize = size, tTimeout = timeout;
    uint8_t *tmpPtr = pData;

    /* no data? */
    if((size == 0) || (!pData)) {return (ESP_RES_ERROR);}
    /* send */
    while(relSize)
    {
        if(relSize / ESP_PACKET_SIZE) {sendSize = ESP_PACKET_SIZE;}//>2048
        else {sendSize = relSize;}
        /* send data */
        esp_SendData(tmpPtr, sendSize);
        while((neverTimeout) || (tTimeout--))
        {
            if(IsEspTxCplt)
            {
                tTimeout = timeout;
                break;
            }

            if((!neverTimeout) && (tTimeout == 0))
            {
                return (ESP_RES_TIMEOUT);
            }
        }
        /* delay */
        TimDelayMs(20);
        /* refresh */
        relSize -= sendSize;
        tmpPtr += sendSize;
    }
    
    return (ESP_RES_OK);  
}

uint8_t ESP8266_StopPassThrough(void)
{
    esp_SendATCmd(CMD_PCIPSENDEND);
    TimDelayMs(5);

    return (ESP_RES_OK);
}

/********* PRIVATE FUNCTION ***************/
static void esp_IoInit(void)
{
	GPIO_InitTypeDef GPIO_Init;

	__HAL_RCC_GPIOI_CLK_ENABLE();
	GPIO_Init.Pin = ESP_REST_PIN;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_MEDIUM;

	HAL_GPIO_Init(ESP_REST_PORT, &GPIO_Init);
}

static void esp_SendData(uint8_t *pData, uint16_t size)
{
    IsEspTxCplt = FALSE;
    ATKUart_TransmitIT(pData, size);
}

static void esp_SendATCmd(const char *atcmd, ...)
{
    uint16_t size;
    va_list args;

    va_start(args, atcmd);
    vsnprintf((char*)EspCmdBuffer, ESP_CMDBUFF_SIZE, atcmd, args);
    va_end(args);
    /* check data size */
    size = myStrlen((const char *)EspCmdBuffer);
#if 1
    printf("ESP8266 send cmd [%d]: \r\n", size);
    printf("%s\r\n", (const char*)EspCmdBuffer);
#endif
    /* send cmd packet */
    esp_SendData((uint8_t*)EspCmdBuffer, size);
    /* clear rx buffer, wait response */
    IsEspRxCplt = FALSE;
    EspRxDataSize = 0;
    EspRxByteIdx = 0;
}

/* timeout in ms */
static uint8_t esp_WaitResponse(uint8_t *pBuff, uint16_t pSize, const char *response, uint32_t timeout)
{
    uint32_t tmpSize = 0;
    char *tmpPtr = NULL;
    uint8_t neverTimeout = (timeout > 0) ? 0 : 1;
    uint8_t ret = ESP_RES_TIMEOUT;

    while((neverTimeout) || (timeout--))
    {
        if(IsEspRxCplt)
        {
            IsBufferBusy = TRUE;
            /* check */
            if(!pBuff && !response) {ret = ESP_RES_OK;}
            /* get return param */
            if(pBuff)
            {
            	//printf("get param\r\n");
            	tmpPtr = myStrstr((char *)EspDataBuffer, "\r\n");
            	tmpSize = (uint32_t)tmpPtr - (uint32_t)EspDataBuffer;
                tmpSize = (tmpSize < pSize) ? tmpSize : pSize;
                myMemcpy(pBuff, (const uint8_t *)EspDataBuffer, tmpSize);
                pBuff[tmpSize] = '\0';
            }
            /* check response */
            if(response)
            {
            	//printf("check response : %s\r\n", response);
                tmpPtr = myStrstr((char *)EspDataBuffer + tmpSize, response);
                if(tmpPtr != NULL) {ret = ESP_RES_OK;}//error none
                else {ret = ESP_RES_ERROR;}//error
            }
			/* clear rx status */
            myMemset(EspDataBuffer, 0, EspRxDataSize);
			IsBufferBusy = FALSE; 
			IsEspRxCplt = FALSE;
			EspRxDataSize = 0;
    		EspRxByteIdx = 0;
        }
        /* delay */
        TimDelayMs(1);
        /* return */
        if(ret != ESP_RES_TIMEOUT) {return (ret);}
    }
    
    return (ret);
}

static void esp_RxParser(uint8_t data)
{
    if(IsBufferBusy == FALSE)
    {
        EspDataBuffer[EspRxByteIdx] = data;
        /* over size */
        if(++EspRxByteIdx == ESP_DATABUFF_SIZE)
        {
        	EspDataBuffer[EspRxByteIdx-2] = '\r';
        	EspDataBuffer[EspRxByteIdx-1] = '\n';
            IsEspRxCplt = TRUE;
            EspRxDataSize = EspRxByteIdx;
            EspRxByteIdx = 0;
		#if 0         
    		printf("ESP8266 receive [ovs-%d]: \r\n", EspRxDataSize);
    		printf("%s", (const char*)EspDataBuffer);
		#endif    
        }
    }
}

static void esp_RxCpltProc(void)
{
	if(IsEspRxCplt == FALSE)
	{
	    IsEspRxCplt = TRUE;
	    EspRxDataSize = EspRxByteIdx;
	    EspRxByteIdx = 0;
#if 0         
    	printf("ESP8266 receive [%d]: \r\n", EspRxDataSize);
    	printf("%s", (const char*)EspDataBuffer);
#endif
    }
}

static void esp_TxCpltProc(void)
{
    IsEspTxCplt = FALSE;
}

static void esp_ErrorProc(void)
{
    
}

