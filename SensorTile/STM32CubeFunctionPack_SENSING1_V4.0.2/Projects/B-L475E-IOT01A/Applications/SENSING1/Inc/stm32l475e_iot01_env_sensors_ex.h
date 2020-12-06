/**
  ******************************************************************************
  * @file    stm32l475e_iot01_env_sensors_ex.h
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   This file provides a set of functions needed to manage the environmental sensors
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
#ifndef STM32L475E_IOT01_ENV_SENSORS_EX_H
#define STM32L475E_IOT01_ENV_SENSORS_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_env_sensors.h"

/* Exported functions --------------------------------------------------------*/
extern int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance);
extern int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status);

#ifdef __cplusplus
}
#endif

#endif /* STM32L475E_IOT01_ENV_SENSORS_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
