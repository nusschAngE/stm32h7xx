/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32h7xx_it.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    29-December-2017
  * @brief   Main Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_it.h"
#include "stm32h7xx.h"
#include "delay.h"
#include "ucos_ii.h"
/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    volatile unsigned int HFSR[3];
    /* Go to infinite loop when Hard Fault exception occurs */
    HFSR[0] = *((volatile unsigned int *)0xE000ED2C);//hard fault
    HFSR[1] = *((volatile unsigned int *)0xE000ED28);//memory fault address MMSR
    HFSR[2] = *((volatile unsigned int *)0xE000ED29);//bus fault
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    volatile unsigned int BFSR[2];
    /* Go to infinite loop when Memory Manage exception occurs */
    BFSR[0] = *((volatile unsigned int *)0xE000ED29);//bus fault
    BFSR[1] = *((volatile unsigned int *)0xE000ED38);//bus fault address
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    volatile unsigned int MFSR[2];
    /* Go to infinite loop when Bus Fault exception occurs */
    MFSR[0] = *((volatile unsigned int *)0xE000ED28);//memory fault address MMSR
    MFSR[1] = *((volatile unsigned int *)0xE000ED34);//memory fault address MMAR
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//#if (!RTOS_uCOS_II)
//void PendSV_Handler(void)
//{
//
//}
//#endif

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
#ifdef RTOS_uCOS_II

#if OS_CRITICAL_METHOD == 3u                                    /* Allocate storage for CPU status register             */
    OS_CPU_SR  cpu_sr;
#endif
    //OS_ENTER_CRITICAL();
    OSIntEnter();                                               /* Tell uC/OS-II that we are starting an ISR            */
	//OS_EXIT_CRITICAL();
    OSTimeTick();                                               /* Call uC/OS-II's OSTimeTick()                         */
    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */

	
#endif //RTOS_uCOS_II
	HAL_IncTick();
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/