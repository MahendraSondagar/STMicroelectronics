/**
  ******************************************************************************
  * @file    stm32l475e_iot01_bus_ex.c
  * @author  MCD Application Team
  * @brief   STM32L475E-IOT01 board support package bus drivers Extension
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_bus_ex.h"

/* Prototypes ----------------------------------------------------------------*/

static void SPI3_MspInit(SPI_HandleTypeDef* hspi);
static void SPI3_MspDeInit(SPI_HandleTypeDef* hspi);
static uint32_t SPI_GetPrescaler(uint32_t clock_src_hz, uint32_t baudrate_mbps);

/* Local Variables -----------------------------------------------------------*/

SPI_HandleTypeDef hbus_spi3;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
static uint32_t IsSpi3MspCbValid = 0;
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

/* Exported Functions --------------------------------------------------------*/

/**
  * @brief  Initializes MX SPI3 HAL.
  * @param  phspi SPI handler
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef* phspi, uint32_t baudrate_presc)
{
  HAL_StatusTypeDef ret = HAL_OK;

  phspi->Init.Mode              = SPI_MODE_MASTER;
  phspi->Init.Direction         = SPI_DIRECTION_2LINES;
  phspi->Init.DataSize          = SPI_DATASIZE_8BIT;
  phspi->Init.CLKPolarity       = SPI_POLARITY_LOW;
  phspi->Init.CLKPhase          = SPI_PHASE_1EDGE;
  phspi->Init.NSS               = SPI_NSS_SOFT;
  phspi->Init.BaudRatePrescaler = baudrate_presc;
  phspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
  phspi->Init.TIMode            = SPI_TIMODE_DISABLE;
  phspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  phspi->Init.CRCPolynomial     = 7;

  if(HAL_SPI_Init(phspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

/**
  * @brief  Initializes SPI HAL.
  * @retval BSP status
  */
int32_t BSP_SPI3_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_spi3.Instance  = BUS_SPI3_INSTANCE;

  if (HAL_SPI_GetState(&hbus_spi3) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI3_MspInit(&hbus_spi3);
#else
    if(IsSpi3MspCbValid == 0U)
    {
      if(BSP_SPI3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    /* Init the SPI */
    if (MX_SPI3_Init(&hbus_spi3, SPI_GetPrescaler(HAL_RCC_GetPCLK2Freq(), BUS_SPI3_BAUDRATE)) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInitializes SPI HAL.
  * @retval BSP status
  */
int32_t BSP_SPI3_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI3_MspDeInit(&hbus_spi3);
#endif
  /* DeInit the SPI*/
  if(HAL_SPI_DeInit(&hbus_spi3) == HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

/**
  * @brief  Write Data through SPI BUS.
  * @param  pData  Pointer to data buffer to send
  * @param  Length Length of data in byte
  * @retval BSP status
  */
int32_t BSP_SPI3_Send(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbus_spi3, pData, Length, 0x1000) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
  * @brief  Receive Data from SPI BUS
  * @param  pData  Pointer to data buffer to receive
  * @param  Length Length of data in byte
  * @retval BSP status
  */
int32_t  BSP_SPI3_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbus_spi3, pData, Length, 0x1000) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}


/**
  * @brief  Send and Receive data to/from SPI BUS (Full duplex)
  * @param  pData  Pointer to data buffer to send/receive
  * @param  Length Length of data in byte
  * @retval BSP status
  */
int32_t BSP_SPI3_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbus_spi3, pTxData,pRxData, Length, 0x1000) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default SPI3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterDefaultMspCallbacks (void)
{
  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi3);

  /* Register MspInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPINIT_CB_ID, SPI3_MspInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPDEINIT_CB_ID, SPI3_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsSpi3MspCbValid = 1;

  return BSP_ERROR_NONE;
}

/**
  * @brief Register SPI3 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI3 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterMspCallbacks(BSP_SPI_Cb_t *Callbacks)
{
  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi3);

  /* Register MspInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if(HAL_SPI_RegisterCallback(&hbus_spi3, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsSpi3MspCbValid = 1;

  return BSP_ERROR_NONE;
}
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Initializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
  */
static void SPI3_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef gpio_init;

  /* Peripheral clock enable */
  BUS_SPI3_CLK_ENABLE();

  /* Enable GPIO clock */
  BUS_SPI3_SCK_GPIO_CLK_ENABLE();
  BUS_SPI3_MISO_GPIO_CLK_ENABLE();
  BUS_SPI3_MOSI_GPIO_CLK_ENABLE();

  /* SPI3 GPIO Configuration */
  gpio_init.Pin = BUS_SPI3_SCK_GPIO_PIN | BUS_SPI3_MISO_GPIO_PIN | BUS_SPI3_MOSI_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = BUS_SPI3_SCK_GPIO_AF;
  HAL_GPIO_Init(BUS_SPI3_SCK_GPIO_PORT, &gpio_init);
}

/**
  * @brief  DeInitializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
  */
static void SPI3_MspDeInit(SPI_HandleTypeDef* hspi)
{
  /* Peripheral clock disable */
  BUS_SPI3_CLK_DISABLE();

  /* DeInitialize Peripheral GPIOs */
  HAL_GPIO_DeInit(BUS_SPI3_SCK_GPIO_PORT, BUS_SPI3_SCK_GPIO_PIN | BUS_SPI3_MISO_GPIO_PIN | BUS_SPI3_MOSI_GPIO_PIN);
}

/**
  * @brief  Convert the SPI baudrate into prescaler.
  * @param  clock_src_hz : SPI source clock in HZ.
  * @param  baudrate_mbps : SPI baud rate in mbps.
  * @retval Prescaler dividor
  */
static uint32_t SPI_GetPrescaler( uint32_t clock_src_hz, uint32_t baudrate_mbps )
{
  uint32_t divisor = 0;
  uint32_t spi_clk = clock_src_hz;
  uint32_t presc = 0;

  static const uint32_t baudrate[]=
  {
    SPI_BAUDRATEPRESCALER_2,
    SPI_BAUDRATEPRESCALER_4,
    SPI_BAUDRATEPRESCALER_8,
    SPI_BAUDRATEPRESCALER_16,
    SPI_BAUDRATEPRESCALER_32,
    SPI_BAUDRATEPRESCALER_64,
    SPI_BAUDRATEPRESCALER_128,
    SPI_BAUDRATEPRESCALER_256,
  };

  while( spi_clk > baudrate_mbps)
  {
    presc = baudrate[divisor];
    if (++divisor > 7)
      break;

    spi_clk= ( spi_clk >> 1);
  }

  return presc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
