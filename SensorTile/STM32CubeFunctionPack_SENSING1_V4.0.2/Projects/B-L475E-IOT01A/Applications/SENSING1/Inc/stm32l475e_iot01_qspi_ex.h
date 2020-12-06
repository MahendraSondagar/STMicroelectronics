/**
  ******************************************************************************
  * @file    stm32l475e_iot01_qspi_ex.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l475e_iot01_qspi_ex.c driver.
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
#ifndef STM32L475E_IOT01_QSPI_EX_H
#define STM32L475E_IOT01_QSPI_EX_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_qspi.h"

/* Exported functions --------------------------------------------------------*/
extern int32_t BSP_QSPIEx_WaitForEndOfOperation(uint32_t Instance, uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* STM32L475E_IOT01_QSPI_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
