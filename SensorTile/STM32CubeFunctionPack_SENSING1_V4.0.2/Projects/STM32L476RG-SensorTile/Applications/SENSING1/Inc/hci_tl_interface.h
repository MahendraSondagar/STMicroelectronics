/**
  ******************************************************************************
  * @file    hci_tl_interface.h
  * @author  SRA - Central Labs
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   This file contains all the functions prototypes for the STM32
  *          BlueNRG HCI Transport Layer interface
  ******************************************************************************
  *
  * COPYRIGHT 2018 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HCI_TL_INTERFACE_H
#define __HCI_TL_INTERFACE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "bsp.h"



/* Exported Defines ----------------------------------------------------------*/
#if defined(USE_STM32L4XX_NUCLEO)

  #define HCI_TL_SPI_EXTI_PORT  GPIOA
  #define HCI_TL_SPI_EXTI_PIN   GPIO_PIN_0
  #define HCI_TL_SPI_EXTI_IRQn  EXTI0_IRQn

  #define HCI_TL_SPI_IRQ_PORT   GPIOA
  #define HCI_TL_SPI_IRQ_PIN    GPIO_PIN_0

  #define HCI_TL_SPI_CS_PORT    GPIOA
  #define HCI_TL_SPI_CS_PIN     GPIO_PIN_1

  #define HCI_TL_RST_PORT       GPIOA
  #define HCI_TL_RST_PIN        GPIO_PIN_8

  /* SPI re-mapping functions */
  #define BSP_SPI_Init BSP_SPI1_Init
  #define BSP_SPI_SendRecv BSP_SPI1_SendRecv

#elif defined(STM32_SENSORTILE)

  #define HCI_TL_SPI_EXTI_PORT  GPIOC
  #define HCI_TL_SPI_EXTI_PIN   GPIO_PIN_5
  #define HCI_TL_SPI_EXTI_IRQn  EXTI9_5_IRQn

  #define HCI_TL_SPI_IRQ_PORT   GPIOC
  #define HCI_TL_SPI_IRQ_PIN    GPIO_PIN_5

  #define HCI_TL_SPI_CS_PORT    GPIOB
  #define HCI_TL_SPI_CS_PIN     GPIO_PIN_2

  #define HCI_TL_RST_PORT       GPIOH
  #define HCI_TL_RST_PIN        GPIO_PIN_0

  /* SPI re-mapping functions */
  #define BSP_SPI_Init BSP_SPI1_Init
  #define BSP_SPI_SendRecv BSP_SPI1_SendRecv

#elif defined(USE_STM32L475E_IOT01)

  #define HCI_TL_SPI_EXTI_PORT  GPIOE
  #define HCI_TL_SPI_EXTI_PIN   GPIO_PIN_6
  #define HCI_TL_SPI_EXTI_IRQn  EXTI9_5_IRQn

  #define HCI_TL_SPI_IRQ_PORT   GPIOE
  #define HCI_TL_SPI_IRQ_PIN    GPIO_PIN_6

  #define HCI_TL_SPI_CS_PORT    GPIOD
  #define HCI_TL_SPI_CS_PIN     GPIO_PIN_13

  #define HCI_TL_RST_PORT       GPIOA
  #define HCI_TL_RST_PIN        GPIO_PIN_8

  /* SPI re-mapping functions */
  #define BSP_SPI_Init BSP_SPI3_Init
  #define BSP_SPI_SendRecv BSP_SPI3_SendRecv

#elif defined (STM32_SENSORTILEBOX)

  #define HCI_TL_SPI_EXTI_PORT  GPIOD
  #define HCI_TL_SPI_EXTI_PIN   GPIO_PIN_4
  #define HCI_TL_SPI_EXTI_IRQn  EXTI4_IRQn

  #define HCI_TL_SPI_IRQ_PORT   GPIOD
  #define HCI_TL_SPI_IRQ_PIN    GPIO_PIN_4

  #define HCI_TL_SPI_CS_PORT    GPIOD
  #define HCI_TL_SPI_CS_PIN     GPIO_PIN_0

  #define HCI_TL_RST_PORT       GPIOA
  #define HCI_TL_RST_PIN        GPIO_PIN_8
     
  /* SPI re-mapping functions */
  #define BSP_SPI_Init BSP_SPI2_Init
  #define BSP_SPI_SendRecv BSP_SPI2_SendRecv

#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */

/* Exported Functions --------------------------------------------------------*/
int32_t HCI_TL_SPI_Init    (void* pConf);
int32_t HCI_TL_SPI_DeInit  (void);
int32_t HCI_TL_SPI_Receive (uint8_t* buffer, uint16_t size);
int32_t HCI_TL_SPI_Send    (uint8_t* buffer, uint16_t size);
int32_t HCI_TL_SPI_Reset   (void);

/**
 * @brief  Register hci_tl_interface IO bus services
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_init(void);

/**
 * @brief HCI Transport Layer Low Level Interrupt Service Routine
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_isr(void);

#ifdef __cplusplus
}
#endif
#endif /* __HCI_TL_INTERFACE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
