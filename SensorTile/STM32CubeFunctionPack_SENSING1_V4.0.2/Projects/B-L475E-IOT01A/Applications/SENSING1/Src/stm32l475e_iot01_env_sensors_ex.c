/**
  ******************************************************************************
  * @file    stm32l475e_iot01_env_sensors_ex.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   This file provides a set of functions needed to manage the environmental sensors extended 
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
#include "stm32l475e_iot01_env_sensors_ex.h"

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Set environmental sensor one shot mode
 * @param  Instance environmental sensor instance to be used
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Instance == 0U)
    {
      if (HTS221_Set_One_Shot(Env_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      if (LPS22HB_Set_One_Shot(Env_Sensor_CompObj[Instance]) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return status;
}

/**
 * @brief  Get environmental sensor one shot status
 * @param  Instance environmental sensor instance to be used
 * @param  Status pointer to the one shot status (1 means measurements available, 0 means measurements not available yet)
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Instance == 0U)
    {
      if (HTS221_Get_One_Shot_Status(Env_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      if (LPS22HB_Get_One_Shot_Status(Env_Sensor_CompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return status;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
