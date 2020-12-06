/******************************************************************************
 * @file    main.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    30-April-2018
 * @brief   this is the main!
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
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

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "low_power_manager.h"
#include "scheduler.h"
#include "st_sigfox_api.h"
#include "radio.h"
#include "command.h"
#include "hw_eeprom.h"
#include "at.h"
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sfx_u8 error = 0; 
uint8_t err_id;

/* Private functions ---------------------------------------------------------*/
static sfx_error_t st_sigfox_open( void );
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main( void )
{
  /* STM32 HAL library initialization*/
  HAL_Init( );
  /* Configure the system clock*/
  SystemClock_Config( );
  /* Configure the debug mode*/
  /* BE AWARE: if DEBUG flag is not defined (see hw_config.h) */
  /* by executing DBG_Init() if attached the debbuger will DISCONNECT !!!!   */
  /* The board keeps running, accessible via AT commands */
  DBG_Init();
  /* Configure the hardware*/
  HW_Init();
  /* Initialise Eeprom factory SEting at device Birth*/
  HW_EEPROM_Init();

  CMD_Init();
  /*Disable standby mode*/
  LPM_SetOffMode(LPM_APPLI_Id, LPM_Disable);

  BSP_LED_Init( LED_BLUE );
  BSP_LED_Init( LED_GREEN );
  BSP_LED_Init( LED_RED2 );

  /*OPen Sifox Lib*/
  st_sigfox_open();

  SCH_RegTask( VCOM_TASK, CMD_Process );

  /* main loop*/
  while( 1 )
  {
    SCH_Run( );
  }
}

void SCH_Idle( void )
{
  BACKUP_PRIMASK();

  DISABLE_IRQ( );

  LPM_EnterLowPower( );

  RESTORE_PRIMASK( );
}

static sfx_error_t st_sigfox_open( void )
{
  st_sfx_rc_t SgfxRc = E2pData.SgfxRc;
  sfx_error_t error = SFX_ERR_NONE;

  /*record RCZ*/
  switch(SgfxRc.id)
  {
    case RC1_ID:
    {
      error = SIGFOX_API_open(&SgfxRc.param);

      break;
    }
    case RC2_ID:
    {
      sfx_u32 config_words[3];

      config_words[0] = E2pData.macroch_config_words_rc2[0];
      config_words[1] = E2pData.macroch_config_words_rc2[1];
      config_words[2] = E2pData.macroch_config_words_rc2[2];
      
      error = SIGFOX_API_open(&SgfxRc.param );

      if ( error == SFX_ERR_NONE )
      {
        error = SIGFOX_API_set_std_config(  config_words, RC2_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    case RC3C_ID:
    {
      sfx_u32 config_words[3];
      
      config_words[0] = E2pData.macroch_config_words_rc3[0];
      config_words[1] = E2pData.macroch_config_words_rc3[1];
      config_words[2] = E2pData.macroch_config_words_rc3[2];


      error = SIGFOX_API_open(&SgfxRc.param );

      if ( error == SFX_ERR_NONE )
      {
        error = SIGFOX_API_set_std_config( config_words, NA);
      }

      break;
    }
    case RC4_ID:
    {
      sfx_u32 config_words[3];
            
      config_words[0] = E2pData.macroch_config_words_rc4[0];
      config_words[1] = E2pData.macroch_config_words_rc4[1];
      config_words[2] = E2pData.macroch_config_words_rc4[2];
    
      error = SIGFOX_API_open(&SgfxRc.param );

      if ( error == SFX_ERR_NONE )
      {
        error = SIGFOX_API_set_std_config( config_words, RC4_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    default:
    {
      error = SFX_ERR_API_OPEN;
      break;
    }
  }

  if (E2pData.AtEcho == E2P_SET)
  {
    if (error == SFX_ERR_NONE)
    {
      AT_PRINTF("\r\n\n\rSIGFOX AT MODEM READY\n\r\n\r");
    }
    else
    {
      AT_PRINTF("\r\n\n\rSIGFOX AT MODEM ERROR: %d\n\r\n\r", error);
    }
  }
  return error;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
