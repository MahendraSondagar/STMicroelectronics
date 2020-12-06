/**
  ******************************************************************************
  * @file    bsp.h 
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

/* Includes ------------------------------------------------------------------*/
#include "SENSING1.h"
#if defined(USE_STM32L4XX_NUCLEO)
  #include "nucleo_l476rg_bus.h"
  #include "stm32l4xx_nucleo.h"
  #include "iks01a2_motion_sensors.h"
  #include "HWAdvanceFeatures.h"
  #include "stm32l4xx_hal_conf.h"
  #include "stm32l4xx_UART.h"
  #include "stm32l4xx_periph_conf.h"
  #include "iks01a2_env_sensors.h"
  #ifdef ONE_SHOT
    #include "iks01a2_env_sensors_ex.h"
  #endif /* ONE_SHOT */
  #include "iks01a2_motion_sensors.h"
  #include "iks01a2_motion_sensors_ex.h"
  #include "x_nucleo_cca02m1_audio_patch.h"
#elif defined(STM32_SENSORTILE)
  #include "SensorTile.h"
  #include "SensorTile_conf.h"
  #include "SensorTile_motion_sensors.h"
  #include "HWAdvanceFeatures.h"
  
  #include "stm32l4xx_hal_conf.h"
  #include "stm32l4xx_hal_def.h"
  #include "hci_tl_interface.h"
  #include "SensorTile_env_sensors.h"
  #ifdef ONE_SHOT
    #include "SensorTile_env_sensors_ex.h"
  #endif /* ONE_SHOT */
  #include "SensorTile_motion_sensors_ex.h"
  #include "SensorTile_gg.h"
  #include "SensorTile_audio.h"
  #include "SensorTile_sd.h"
#elif defined(USE_STM32L475E_IOT01)
  #include "stm32l475e_iot01_conf.h"
  #include "stm32l475e_iot01_bus_ex.h"
  #include "stm32l475e_iot01.h"
  #include "stm32l475e_iot01_motion_sensors.h"
//  #include "HWAdvanceFeatures.h"
  #if SENSING1_USE_PRINTF
     extern UART_HandleTypeDef hcom_uart[COMn];
     #define UartHandle (hcom_uart[COM1])
  #endif /* SENSING1_USE_PRINTF */

  #include "stm32l475e_iot01_audio.h"
  #include "stm32l4xx_hal_conf.h"
  #include "stm32l4xx_hal_def.h"
  #include "hci_tl_interface.h"
  #include "stm32l475e_iot01_env_sensors.h"
  #ifdef ONE_SHOT
    #include "stm32l475e_iot01_env_sensors_ex.h"
  #endif /* ONE_SHOT */
#elif defined (STM32_SENSORTILEBOX)
  #include "SensorTile.box.h"
  #include "stm32l4xx_hal_conf.h"
  #include "SensorTile.box_env_sensors.h"
  #include "SensorTile.box_env_sensors_ex.h"
  #include "SensorTile.box_motion_sensors.h"
  #include "SensorTile.box_motion_sensors_ex.h"
  #include "SensorTile.box_audio.h"
  #include "SensorTile.box_bc.h"
  #include "HWAdvanceFeatures.h"

  #if SENSING1_USE_DATALOG
     #include "SensorTile.box_sd.h"
  #endif /* SENSING1_USE_DATALOG */
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */

#endif /* __BSP_H */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
