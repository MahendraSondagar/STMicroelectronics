/**
  ******************************************************************************
  * @file    MEMS_FuncRemapping.h
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Macros for remapping the MEMS' functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEMS_FUNCREMAPPING_H
#define __MEMS_FUNCREMAPPING_H

/* Exported defines ------------------------------------------------------------*/
#define MOTION_SENSOR_AxesRaw_t BSP_MOTION_SENSOR_AxesRaw_t
#define MOTION_SENSOR_Axes_t    BSP_MOTION_SENSOR_Axes_t

#define MOTION_SENSOR_GetAxes    BSP_MOTION_SENSOR_GetAxes
#define MOTION_SENSOR_GetAxesRaw BSP_MOTION_SENSOR_GetAxesRaw

#define ENV_SENSOR_GetValue BSP_ENV_SENSOR_GetValue
#define ENV_SENSOR_Set_One_Shot BSP_ENV_SENSOR_Set_One_Shot

#define MOTION_SENSOR_GetOutputDataRate BSP_MOTION_SENSOR_GetOutputDataRate
#define MOTION_SENSOR_SetOutputDataRate BSP_MOTION_SENSOR_SetOutputDataRate

#define MOTION_SENSOR_Enable_6D_Orientation BSP_MOTION_SENSOR_Enable_6D_Orientation
#define MOTION_SENSOR_Disable_6D_Orientation BSP_MOTION_SENSOR_Disable_6D_Orientation
#define MOTION_SENSOR_Get_6D_Orientation_XL BSP_MOTION_SENSOR_Get_6D_Orientation_XL
#define MOTION_SENSOR_Get_6D_Orientation_XH BSP_MOTION_SENSOR_Get_6D_Orientation_XH
#define MOTION_SENSOR_Get_6D_Orientation_YL BSP_MOTION_SENSOR_Get_6D_Orientation_YL
#define MOTION_SENSOR_Get_6D_Orientation_YH BSP_MOTION_SENSOR_Get_6D_Orientation_YH
#define MOTION_SENSOR_Get_6D_Orientation_ZL BSP_MOTION_SENSOR_Get_6D_Orientation_ZL
#define MOTION_SENSOR_Get_6D_Orientation_ZH BSP_MOTION_SENSOR_Get_6D_Orientation_ZH

#define MOTION_SENSOR_Enable_Tilt_Detection BSP_MOTION_SENSOR_Enable_Tilt_Detection
#define MOTION_SENSOR_Disable_Tilt_Detection BSP_MOTION_SENSOR_Disable_Tilt_Detection

#define MOTION_SENSOR_Enable_Wake_Up_Detection BSP_MOTION_SENSOR_Enable_Wake_Up_Detection
#define MOTION_SENSOR_Disable_Wake_Up_Detection BSP_MOTION_SENSOR_Disable_Wake_Up_Detection

#define MOTION_SENSOR_Enable_Free_Fall_Detection BSP_MOTION_SENSOR_Enable_Free_Fall_Detection
#define MOTION_SENSOR_Disable_Free_Fall_Detection BSP_MOTION_SENSOR_Disable_Free_Fall_Detection
#define MOTION_SENSOR_Set_Free_Fall_Threshold BSP_MOTION_SENSOR_Set_Free_Fall_Threshold

#define MOTION_SENSOR_Enable_Wake_Up_Detection BSP_MOTION_SENSOR_Enable_Wake_Up_Detection
#define MOTION_SENSOR_Disable_Wake_Up_Detection BSP_MOTION_SENSOR_Disable_Wake_Up_Detection

#define MOTION_SENSOR_Enable_Double_Tap_Detection BSP_MOTION_SENSOR_Enable_Double_Tap_Detection
#define MOTION_SENSOR_Disable_Double_Tap_Detection BSP_MOTION_SENSOR_Disable_Double_Tap_Detection
#define MOTION_SENSOR_Enable_Single_Tap_Detection BSP_MOTION_SENSOR_Enable_Single_Tap_Detection
#define MOTION_SENSOR_Disable_Single_Tap_Detection BSP_MOTION_SENSOR_Disable_Single_Tap_Detection
#define MOTION_SENSOR_Set_Tap_Threshold BSP_MOTION_SENSOR_Set_Tap_Threshold

#define MOTION_SENSOR_Event_Status_t BSP_MOTION_SENSOR_Event_Status_t
#define MOTION_SENSOR_Get_Event_Status BSP_MOTION_SENSOR_Get_Event_Status

//#define MOTION_SENSOR_INT1_PIN MOTION_SENSOR_INT1_PIN
//#define MOTION_SENSOR_INT2_PIN MOTION_SENSOR_INT2_PIN

#define MOTION_SENSOR_Enable_Pedometer BSP_MOTION_SENSOR_Enable_Pedometer
#define MOTION_SENSOR_Disable_Pedometer BSP_MOTION_SENSOR_Disable_Pedometer
#define MOTION_SENSOR_Reset_Step_Counter BSP_MOTION_SENSOR_Step_Counter_Reset
#define MOTION_SENSOR_Get_Step_Count BSP_MOTION_SENSOR_Get_Step_Count

#define MOTION_SENSOR_Enable BSP_MOTION_SENSOR_Enable
#define MOTION_SENSOR_Disable BSP_MOTION_SENSOR_Disable
#define MOTION_SENSOR_Init BSP_MOTION_SENSOR_Init
#define MOTION_SENSOR_SetFullScale BSP_MOTION_SENSOR_SetFullScale
#define MOTION_SENSOR_GetSensitivity BSP_MOTION_SENSOR_GetSensitivity

#define ENV_SENSOR_Enable BSP_ENV_SENSOR_Enable
#define ENV_SENSOR_Disable BSP_ENV_SENSOR_Disable
#define ENV_SENSOR_Init BSP_ENV_SENSOR_Init

#define AUDIO_INSTANCE BSP_AUDIO_IN_INSTANCE
#define AUDIO_DFSDM_DMAx_MIC1_IRQHandler DMA1_Channel4_IRQHandler 

#endif /* __MEMS_FUNCREMAPPING_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

