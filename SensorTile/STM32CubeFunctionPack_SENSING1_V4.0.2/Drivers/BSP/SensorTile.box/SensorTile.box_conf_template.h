/**
******************************************************************************
* @file    SensorTile.box_conf_template.h
* @author  SRA - Central Labs
* @version V1.2.0
* @date    01-Sept-2019
* @brief   This file define the SensorTile.box configuration
*          This file should be copied to the application folder and renamed
*          to SensorTile.box_conf.h.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
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
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORTILE_BOX_CONF_H__
#define __SENSORTILE_BOX_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "SensorTile.box_bus.h"
#include "SensorTile.box_errno.h"

#define USE_MOTION_SENSOR_LIS2DW12_0        0U
#define USE_MOTION_SENSOR_LIS2MDL_0         1U
#define USE_MOTION_SENSOR_LIS3DHH_0         0U
#define USE_MOTION_SENSOR_LSM6DSOX_0        1U
#define USE_ENV_SENSOR_HTS221_0             1U
#define USE_ENV_SENSOR_LPS22HH_0            1U
#define USE_ENV_SENSOR_STTS751_0            0U
#define USE_AUDIO_IN                        1U
  
//set to 1 if you want to use the implemented irq handler, 0 to use the one in your project files
#define USE_BC_IRQ_HANDLER                  0

//set to 1 if you want to use the implemented irq callback, 0 to use the one in your project files
#define USE_BC_IRQ_CALLBACK                 0

#define AUDIO_SAMPLING_FREQUENCY  16000
#define AUDIO_VOLUME_INPUT      50U
#define BSP_AUDIO_IN_IT_PRIORITY 6U

#define BSP_AUDIO_IN_INSTANCE 1U   /* Audio instance */

#ifdef __cplusplus
}
#endif

#endif /* __SENSORTILE_BOX_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

