/**
 ******************************************************************************
 * @file    lps22hh_ex.h
 * @author  SRA - Central Labs
 * @version V1.0.0
 * @date    13-Feb-2019
 * @brief   LPS22HH Extended header driver file
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
#ifndef LPS22HH_EX_H
#define LPS22HH_EX_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "lps22hh_reg.h"
#include "lps22hh.h"
#include <string.h>

/* Exported defines   --------------------------------------------------------*/

/** LPS22HH low noise mode  **/
#define LPS22HH_LOW_NOISE_DIS      0
#define LPS22HH_LOW_NOISE_EN       1

/* Exported functions --------------------------------------------------------*/
int32_t LPS22HH_Enable_DRDY_Interrupt(LPS22HH_Object_t *pObj);
int32_t LPS22HH_Set_Power_Mode(LPS22HH_Object_t *pObj, uint8_t powerMode);
int32_t LPS22HH_Set_Filter_Mode(LPS22HH_Object_t *pObj, uint8_t filterMode);

#ifdef __cplusplus
}
#endif

#endif /* LPS22HH_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
