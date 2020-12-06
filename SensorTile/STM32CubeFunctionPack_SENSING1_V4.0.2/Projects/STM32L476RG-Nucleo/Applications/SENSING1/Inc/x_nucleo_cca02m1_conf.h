/**
 ******************************************************************************
 * @file    x_nucleo_cca02m1_conf.h
 * @author  Central Labs
  * @version V4.0.0
  * @date    30-Oct-2019
 * @brief   This file contains definitions for the MEMSMIC1 applications
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
#ifndef __X_NUCLEO_CCA02M1_CONF_H__
#define __X_NUCLEO_CCA02M1_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#ifdef USE_STM32F4XX_NUCLEO
#include "stm32f4xx_hal.h"
#define BSP_AUDIO_INSTANCE 0U
#endif
#ifdef USE_STM32L4XX_NUCLEO
#include "stm32l4xx_hal.h"
#define BSP_AUDIO_INSTANCE 1U
#endif
#ifdef USE_STM32F7XX_NUCLEO_144
#include "stm32f7xx_hal.h"
#define BSP_AUDIO_INSTANCE 0U
#endif

#include "nucleo_l476rg_bus.h"
#include "nucleo_l476rg_errno.h"

/* The N_MS value defines the number of millisecond to be processed at each AudioProcess call,
that must be consistent with the N_MS_PER_INTERRUPT defined in the audio driver
(x_nucleo_cca02m1_audio.h).
The default value of the N_MS_PER_INTERRUPT directive in the driver is set to 1, 
for backward compatibility: leaving this values as it is allows to avoid any 
modification in the application layer developed with the older versions of the driver */

#define N_MS (N_MS_PER_INTERRUPT)

#define AUDIO_CHANNELS           1
#define AUDIO_SAMPLING_FREQUENCY 16000

#if (AUDIO_SAMPLING_FREQUENCY == 8000)
#define MAX_DECIMATION_FACTOR 160
#else
#define MAX_DECIMATION_FACTOR 128
#endif
  
  /*#define USE_SPI3*/
  /*If you wanto to use SPI3 instead of SPI2 for M3 and M4, uncomment this define and 
  close SB20 and SB21*/

#ifdef __cplusplus
}
#endif

#endif /* __X_NUCLEO_CCA02M1_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

