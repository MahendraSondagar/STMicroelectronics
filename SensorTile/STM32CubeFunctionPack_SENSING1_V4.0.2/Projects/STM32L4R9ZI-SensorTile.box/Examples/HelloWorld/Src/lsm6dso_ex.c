/**
 ******************************************************************************
 * @file    lsm6dso_ex.c
 * @author  SRA - Central Labs
 * @version V1.0.0
 * @date    13-Feb-2019
 * @brief   LSM6DSO Extended driver file
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
#include "lsm6dso_ex.h"

/* Exported Functions --------------------------------------------------------*/
/**
 * @brief  Enable DRDY interrupt mode
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_ACC_Enable_DRDY_Interrupt(LSM6DSO_Object_t *pObj)
{
  lsm6dso_pin_int1_route_t pin_int1_route;
  
  /* Enable accelerometer DRDY Interrupt on INT1 */
  if(lsm6dso_pin_int1_route_get(&(pObj->Ctx), &pin_int1_route) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  } 
  pin_int1_route.int1_ctrl.int1_drdy_xl = 1;
  pin_int1_route.int1_ctrl.int1_drdy_g = 0;
  if(lsm6dso_pin_int1_route_set(&(pObj->Ctx), &pin_int1_route) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  } 
  
  return LSM6DSO_OK;
}

/**
 * @brief  Enable DRDY interrupt mode
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_GYRO_Enable_DRDY_Interrupt(LSM6DSO_Object_t *pObj)
{
  lsm6dso_pin_int2_route_t pin_int2_route;  
  
  /* Enable gyroscope DRDY Interrupts on INT2 */
  if(lsm6dso_pin_int2_route_get(&(pObj->Ctx), &pin_int2_route) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  } 
  pin_int2_route.int2_ctrl.int2_drdy_xl = 0;
  pin_int2_route.int2_ctrl.int2_drdy_g = 1;
  if(lsm6dso_pin_int2_route_set(&(pObj->Ctx), &pin_int2_route) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }
  
  return LSM6DSO_OK;
}

/**
 * @brief  Set the LSM6DSO accelerometer power mode
 * @param  pObj the device pObj
 * @param  acc_gyro_flag: =0 to set acc powermode; =1 to set gyro powermode
 * @param  Value of the powerMode
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_ACC_GYRO_Set_Power_Mode(LSM6DSO_Object_t *pObj, uint8_t acc_gyro_flag, uint8_t powerMode)
{
  if(acc_gyro_flag == 0)
  {
    if(lsm6dso_xl_power_mode_set(&(pObj->Ctx), (lsm6dso_xl_hm_mode_t)powerMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
  }
  else
  {
    if(lsm6dso_gy_power_mode_set(&(pObj->Ctx), (lsm6dso_g_hm_mode_t)powerMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
  }
  
  return LSM6DSO_OK;  
}

/**
 * @brief  Set the LSM6DSO accelerometer filter mode
 * @param  pObj the device pObj
 * @param  acc_gyro_flag: =0 to set acc low-pass filtermode; =1 to set acc high-pass filtermode; =2 to set gyro low-pass_1 filtermode; =3 to set gyro high-pass filtermode
 * @param  Value of the filterMode
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_ACC_GYRO_Set_Filter_Mode(LSM6DSO_Object_t *pObj, uint8_t acc_gyro_flag, uint8_t filterMode)
{
  /*Set accelerometer low_pass filter-mode*/
  if(acc_gyro_flag == 0)
  {
    /*Set to 1 LPF2 bit (CTRL8_XL)*/
    if(lsm6dso_xl_filter_lp2_set(&(pObj->Ctx), 1) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
    if(lsm6dso_xl_hp_path_on_out_set(&(pObj->Ctx), (lsm6dso_hp_slope_xl_en_t)filterMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
  }
  /*Set accelerometer high_pass filter-mode*/
  else if(acc_gyro_flag == 1)
  {
    if(lsm6dso_xl_hp_path_on_out_set(&(pObj->Ctx), (lsm6dso_hp_slope_xl_en_t)filterMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
  }
  /*Set gyroscope low_pass 1 filter-mode*/
  else if(acc_gyro_flag == 2)
  {
    /* Enable low-pass filter */
    if(lsm6dso_gy_filter_lp1_set(&(pObj->Ctx), 1) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
    if(lsm6dso_gy_lp1_bandwidth_set(&(pObj->Ctx), (lsm6dso_ftype_t)filterMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }    
  }  
  /*Set gyroscope high_pass filter-mode*/
  else if(acc_gyro_flag == 3)
  {
    /* Enable high-pass filter */
    if(lsm6dso_gy_hp_path_internal_set(&(pObj->Ctx), (lsm6dso_hpm_g_t)filterMode) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }    
  }
  return LSM6DSO_OK;  
}

/**
 * @brief  Enable inactivity detection
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_ACC_Enable_Inactivity_Detection(LSM6DSO_Object_t *pObj, lsm6dso_inact_en_t inact_mode)
{
  int32_t ret = LSM6DSO_OK;
  lsm6dso_pin_int2_route_t val_reg;

  /* Full scale selection */
  if (LSM6DSO_ACC_SetFullScale(pObj, 2) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }
  if (LSM6DSO_GYRO_SetFullScale(pObj, 250) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }  

  /* SLEEP_DUR setting */
  if (lsm6dso_act_sleep_dur_set(&(pObj->Ctx), 0x01) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }

  /* Enable inactivity detection. */
  switch(inact_mode)
  {
  case LSM6DSO_XL_AND_GY_NOT_AFFECTED:
    if (lsm6dso_act_mode_set(&(pObj->Ctx), LSM6DSO_XL_AND_GY_NOT_AFFECTED) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }
    break;
  case LSM6DSO_XL_12Hz5_GY_NOT_AFFECTED:
    if (lsm6dso_act_mode_set(&(pObj->Ctx), LSM6DSO_XL_12Hz5_GY_NOT_AFFECTED) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }    
    break;
  case LSM6DSO_XL_12Hz5_GY_SLEEP:
    if (lsm6dso_act_mode_set(&(pObj->Ctx), LSM6DSO_XL_12Hz5_GY_SLEEP) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }    
    break;
  case LSM6DSO_XL_12Hz5_GY_PD:
    if (lsm6dso_act_mode_set(&(pObj->Ctx), LSM6DSO_XL_12Hz5_GY_PD) != LSM6DSO_OK)
    {
      return LSM6DSO_ERROR;
    }    
    break;
  }
  val_reg.md2_cfg.int2_sleep_change = PROPERTY_ENABLE;
  if (lsm6dso_pin_int2_route_set(&(pObj->Ctx), &val_reg) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }
  return ret;
}

/**
 * @brief  Set sleep duration
 * @param  pObj the device pObj
 * @param  Duration wake up detection duration
 * @retval 0 in case of success, an error code otherwise
 */
int32_t LSM6DSO_ACC_Set_Sleep_Duration(LSM6DSO_Object_t *pObj, uint8_t Duration)
{
  /* Set wake up duration. */
  if (lsm6dso_act_sleep_dur_set(&(pObj->Ctx), Duration) != LSM6DSO_OK)
  {
    return LSM6DSO_ERROR;
  }

  return LSM6DSO_ERROR;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
