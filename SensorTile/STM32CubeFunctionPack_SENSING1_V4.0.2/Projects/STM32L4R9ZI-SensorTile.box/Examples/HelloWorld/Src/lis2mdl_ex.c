/**
 ******************************************************************************
 * @file    lis2mdl_ex.c
 * @author  SRA - Central Labs
 * @version V1.0.0
 * @date    13-Feb-2019
 * @brief   LIS2MDL Extended driver file
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
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "lis2mdl_ex.h"

/* Exported Functions --------------------------------------------------------*/
/**
 * @brief  Set the LIS2MDL filter mode
 * @param  pObj the device pObj
 * @param  Value of the filterMode
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LIS2MDL_MAG_Set_Filter_Mode(LIS2MDL_Object_t *pObj, uint8_t filterMode)
{
  if(lis2mdl_low_pass_bandwidth_set(&(pObj->Ctx), (lis2mdl_lpf_t)filterMode) != LIS2MDL_OK)
  {
    return LIS2MDL_ERROR;
  }
  
  return LIS2MDL_OK;  
}

/**
 * @brief  Set the LIS2MDL power mode
 * @param  pObj the device pObj
 * @param  Value of the powerMode
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LIS2MDL_MAG_Set_Power_Mode(LIS2MDL_Object_t *pObj, uint8_t powerMode)
{
  if(lis2mdl_power_mode_set(&(pObj->Ctx), (lis2mdl_lp_t)powerMode) != LIS2MDL_OK)
  {
    return LIS2MDL_ERROR;
  }
  
  return LIS2MDL_OK;  
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
