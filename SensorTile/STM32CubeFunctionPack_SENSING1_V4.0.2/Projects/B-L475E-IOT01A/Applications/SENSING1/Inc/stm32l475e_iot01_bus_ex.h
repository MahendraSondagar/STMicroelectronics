/**
  ******************************************************************************
  * @file    stm32l475e_iot01_bus_ex.h
  * @author  MCD Application Team
  * @brief   bus header file Extension
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L475E_IOT01_BUS_EX_H
#define STM32L475E_IOT01_BUS_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_conf.h"
#include "stm32l475e_iot01_errno.h"


#ifndef BUS_SPI3_BAUDRATE
   #define BUS_SPI3_BAUDRATE  10000000U /* baud rate of SPIn = 10 Mbps*/
#endif

#define BUS_SPI3_INSTANCE                       SPI3

#define BUS_SPI3_CLK_ENABLE()                   __HAL_RCC_SPI3_CLK_ENABLE()
#define BUS_SPI3_CLK_DISABLE()                  __HAL_RCC_SPI3_CLK_DISABLE()

#define BUS_SPI3_SCK_GPIO_PIN                   GPIO_PIN_10
#define BUS_SPI3_MISO_GPIO_PIN                  GPIO_PIN_11
#define BUS_SPI3_MOSI_GPIO_PIN                  GPIO_PIN_12

#define BUS_SPI3_SCK_GPIO_PORT                  GPIOC
#define BUS_SPI3_MISO_GPIO_PORT                 GPIOC
#define BUS_SPI3_MOSI_GPIO_PORT                 GPIOC

#define BUS_SPI3_SCK_GPIO_AF                    GPIO_AF6_SPI3
#define BUS_SPI3_MISO_GPIO_AF                   GPIO_AF6_SPI3
#define BUS_SPI3_MOSI_GPIO_AF                   GPIO_AF6_SPI3

#define BUS_SPI3_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUS_SPI3_MISO_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUS_SPI3_MOSI_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

#ifndef BUS_SPI3_POLL_TIMEOUT
  #define BUS_SPI3_POLL_TIMEOUT                   0x1000
#endif

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
}BSP_SPI_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

extern SPI_HandleTypeDef hbus_spi3;

int32_t BSP_SPI3_Init(void);
int32_t BSP_SPI3_DeInit(void);
int32_t BSP_SPI3_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI3_Recv(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI3_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);
int32_t BSP_GetTick(void);
HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef* phspi, uint32_t baudrate_presc);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI3_RegisterDefaultMspCallbacks (void);
int32_t BSP_SPI3_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

#ifdef __cplusplus
}
#endif

#endif /* STM32L475E_IOT01_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
