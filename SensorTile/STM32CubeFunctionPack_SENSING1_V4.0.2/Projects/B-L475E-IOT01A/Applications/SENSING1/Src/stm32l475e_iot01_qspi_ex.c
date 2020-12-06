/**
  ******************************************************************************
  * @file    stm32l475e_iot01_qspi_ex.c
  * @author  MCD Application Team
  * @brief   This file includes functions to extentend the MX25R6435F QSPI
  *          driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "stm32l475e_iot01_qspi.h"

/**
  * @brief  Wait for QSPI operation to complete on memory side.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPIEx_WaitForEndOfOperation(uint32_t Instance, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    do
    {
      ret = BSP_QSPI_GetStatus(Instance);
      if (Timeout != HAL_MAX_DELAY)
      {
        if ((HAL_GetTick() - tickstart) >= Timeout)
        {
          ret = BSP_ERROR_UNKNOWN_FAILURE;
        }
      }
    } while ((ret != BSP_ERROR_NONE) && (ret != BSP_ERROR_COMPONENT_FAILURE));
  }

  /* Return BSP status */
  return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
