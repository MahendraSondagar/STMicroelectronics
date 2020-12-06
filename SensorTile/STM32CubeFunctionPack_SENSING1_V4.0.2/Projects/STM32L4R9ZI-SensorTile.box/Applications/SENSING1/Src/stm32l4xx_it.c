/**
  ******************************************************************************
  * @file    stm32l4xx_it.c 
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
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
#include "TargetFeatures.h"
#include "stm32l4xx_it.h"
#include "cmsis_os.h"
#include "hci_tl_interface.h"

/* Imported variables ---------------------------------------------------------*/

#if SENSING1_USE_DATALOG
  #ifdef STM32_SENSORTILE
    extern SPI_HandleTypeDef SPI_SD_Handle;
  #endif /* STM32_SENSORTILE */
#endif /* SENSING1_USE_DATALOG */

extern RTC_HandleTypeDef RtcHandle;

#if SENSING1_USE_USB
  extern PCD_HandleTypeDef hpcd;
#endif /* SENSING1_USE_USB */


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
  /* Go to infinite loop when Hard Fault exception occurs */
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
  /* Go to infinite loop when Memory Manage exception occurs */
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
  /* Go to infinite loop when Bus Fault exception occurs */
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
#ifdef NO_FREE_RTOS
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}
#endif
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

#ifdef NO_FREE_RTOS
/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
#endif

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  osSystickHandler();
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xxxx.s).                                             */
/******************************************************************************/

#if SENSING1_USE_DATALOG
#ifdef STM32_SENSORTILE
/**
  * @brief  This function handles DMA Tx interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SPI_SD_Handle.hdmatx);
}
#endif /* STM32_SENSORTILE */
#endif /* SENSING1_USE_DATALOG */

/**
  * @brief This function handles DFSDM Left DMA interrupt request.
  * @param None
  * @retval None
  */
void AUDIO_DFSDM_DMAx_MIC1_IRQHandler(void)
{
#if defined(USE_STM32L4XX_NUCLEO)
  HAL_DMA_IRQHandler(&hDmaDfsdm[0]);
#elif defined(STM32_SENSORTILE)
  HAL_DMA_IRQHandler(&hDmaDfsdm);
#elif defined(USE_STM32L475E_IOT01)
  HAL_DMA_IRQHandler(haudio_in_dfsdm_filter[0].hdmaReg);
#elif defined(STM32_SENSORTILEBOX)
  HAL_DMA_IRQHandler(AMic_OnBoard_DfsdmFilter.hdmaReg);
#else
  #error "Define the right interface"
#endif /* USE_STM32L4XX_NUCLEO */
}

#if SENSING1_USE_USB
/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd);
}
#endif /* SENSING1_USE_USB */

#ifdef USE_STM32L4XX_NUCLEO
/**
  * @brief  EXTI0_IRQHandler This function handles External line
  *         interrupt request for BlueNRG.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(HCI_TL_SPI_EXTI_PIN);
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}

/**
 * @brief  This function handles External line 4 interrupt request
 * @param  None
 * @retval None
 */
void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/**
  * @brief  This function handles External line 5-9 interrupt request
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler( void )
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

/**
* @brief  This function handles External line 10-15 interrupt request.
* @param  None
* @retval None
*/
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

#elif defined(STM32_SENSORTILE)

/**
  * @brief  This function handles External line 2 interrupt request
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BSP_LSM6DSM_INT2);
}

/**
  * @brief  EXTI9_5_IRQHandler This function handles External line
  *         interrupt request for BlueNRG.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(HCI_TL_SPI_EXTI_PIN);
}

#elif defined(USE_STM32L475E_IOT01)
/**
  * @brief  EXTI9_5_IRQHandler This function handles External line
  *         interrupt request for BlueNRG.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(HCI_TL_SPI_EXTI_PIN);
}

/**
* @brief  This function handles External line 10-15 interrupt request.
* @param  None
* @retval None
*/
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BUTTON_USER_PIN);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}

#elif defined(STM32_SENSORTILEBOX)

/**
* @brief  This function handles External line 1 interrupt request.
* @param  None
* @retval None
*/
void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

/**
  * @brief  EXTI2_IRQHandler This function handles External line
  *         interrupt request for Power Button
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(POWER_BUTTON_PIN);
}

/**
  * @brief  EXTI3_IRQHandler This function handles External line
  *         interrupt from Battery Charger
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
  /* HW events from LSM6DSOX */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/**
  * @brief  EXTI4_IRQHandler This function handles External line
  *         interrupt request for BlueNRG.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(HCI_TL_SPI_EXTI_PIN);
}

#if SENSING1_USE_DATALOG
/**
* @brief This function handles SMMC1 interrupts.
*/
void SDMMC1_IRQHandler(void)
{
  HAL_SD_IRQHandler(&hsd1);
}
#endif /* SENSING1_USE_DATALOG */

void DFSDM1_FLT1_IRQHandler(void)
{
  HAL_DFSDM_IRQHandler(&AMic_OnBoard_DfsdmFilter);
}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SensorTileADC.DMA_Handle);
}

#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */

/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
