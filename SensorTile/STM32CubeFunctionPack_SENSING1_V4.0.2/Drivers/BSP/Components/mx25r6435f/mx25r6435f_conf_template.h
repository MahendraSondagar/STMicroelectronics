/**
  ******************************************************************************
  * @file    mx25r6435f_conf.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MX25R6435F QSPI memory.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25R6435F_CONF_H
#define MX25R6435F_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32YYxx_hal.h" /* User application to set STM32 series HAL header */

#define DUMMY_CYCLES_READ            8U
#define DUMMY_CYCLES_READ_DUAL       4U
#define DUMMY_CYCLES_READ_QUAD       6U

#ifdef __cplusplus
}
#endif

#endif /* MX25R6435F_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
