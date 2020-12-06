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
#include "sgfx_credentials.h"
#include "hw_eeprom.h"
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* #define STDBY_ON */
#define USER_BUTTON_ALT_PIN                         GPIO_PIN_0
#define USER_BUTTON_ALT_GPIO_PORT                   GPIOA

#define PAC_LEN 8
#define ID_LEN 4
/* Default Configuration zone*/
/* Can be chaged according to application zone*/
#define APPLI_RC   ST_RC1
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sfx_u8 error = 0;
uint8_t err_id;

/* Private functions ---------------------------------------------------------*/
/*!
 * @brief Open the sigfox library
 * @param The Region configuration
 * @retval None
 */
static sfx_error_t st_sigfox_open( st_sfx_rc_t SgfxRc );

/*!
 * @brief send data to back end sigfox server
 * @param None
 * @retval None
 */
static void send_data( void );

/*!
 * @brief to post interrupt to backgroud.
 * @Note managed by scheduler
 * @param None
 * @retval None
 */
#ifndef STDBY_ON 
static void send_data_request( void );
#endif

/*!
 * @brief Initialize the user btton to request sending data
 * @param None
 * @retval None
 */
#ifndef STDBY_ON 
/* when STDBY_ON the reset button is used instead of the push button */
static void user_button_init( void );
#endif

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main( void )
{
  sfx_error_t error;
  
  uint8_t dev_id[ID_LEN];
  
  uint8_t dev_pac[PAC_LEN];

  st_sfx_rc_t SgfxRc=APPLI_RC;

  /* STM32 HAL library initialization*/
  HAL_Init( );
  /* Configure the system clock*/
  SystemClock_Config( );  
  /* Configure the debug mode*/
  DBG_Init( );  
  /* Configure the hardware*/
  HW_Init( );
  /* Initialise Eeprom factory SEting at device Birth*/
  HW_EEPROM_Init();

  BSP_LED_Init( LED_BLUE );
  BSP_LED_Init( LED_GREEN );
  BSP_LED_Init( LED_RED2 );

  PRINTF("wakeup");
  /*OPen Sifox Lib*/
  error=st_sigfox_open( SgfxRc);
  
  /* use private key*/
  HW_EEPROM_WRITE( E2pData.SgfxKey, CREDENTIALS_KEY_PRIVATE);

  if ( error == SFX_ERR_NONE )
  {
    PRINTF(" OK\n\r");
  }
  else
  {
    PRINTF(" error %d\n\r", error);
  }
  /*prints Id and Pac*/
  SIGFOX_API_get_device_id(dev_id);
  SIGFOX_API_get_initial_pac(dev_pac);
  PRINTF("devId=") ; for(int i =0; i<ID_LEN; i++) {PRINTF("%02X",dev_id[ID_LEN-1-i]);} PRINTF("\n\r");
  PRINTF("devPac="); for(int i =0; i<PAC_LEN; i++) {PRINTF("%02X",dev_pac[i]);} PRINTF("\n\r");
  /* Disable stand by mode*/
#ifdef STDBY_ON
  
  send_data();
  
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  
  /*Clear all related wakeup flags*/
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /*Re-enable all used wakeup sources: Pin1(PA.0)*/
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  
  
#else
  LPM_SetOffMode(LPM_APPLI_Id, LPM_Disable);
  /*initialise user button*/
  user_button_init( );
  /* record send data task*/
  SCH_RegTask( SEND_TASK, send_data );
#endif

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

static void send_data( void )
{
  uint8_t ul_msg[12] = {0x00, 0x01, 0x02, 0x03,0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11}; 
  uint8_t dl_msg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t  ul_size =0;
  uint32_t nbTxRepeatFlag=1;
  int i=0;
  uint16_t pressure = 0;
  int16_t temperature = 0;
  uint16_t humidity = 0;
  uint32_t batteryLevel= HW_GetBatteryLevel( );                     /*in mV*/

#if defined(SENSOR_ENABLED)
  sensor_t sensor_data;
  BSP_sensor_Read( &sensor_data );
  temperature = ( int16_t )( sensor_data.temperature * 100 );     /* in °C * 100 */
  pressure    = ( uint16_t )( sensor_data.pressure * 100 / 10 );  /* in hPa / 10 */
  humidity    = ( uint16_t )( sensor_data.humidity * 10 );        /* in %*10     */
#endif  

#ifdef STDBY_ON
  LPM_SetOffMode(LPM_APPLI_Id, LPM_Disable);
#else
  /*disable irq to forbidd user to press button while transmitting*/
  HW_GPIO_SetIrq( USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, 1, NULL );
#endif

  ul_msg[ul_size++] = (uint8_t) ((batteryLevel*100)/3300);
  ul_msg[ul_size++] = ( pressure >> 8 ) & 0xFF;
  ul_msg[ul_size++] = pressure & 0xFF;
  ul_msg[ul_size++] = ( temperature >> 8 ) & 0xFF;
  ul_msg[ul_size++] = temperature & 0xFF;
  ul_msg[ul_size++] = ( humidity >> 8 ) & 0xFF;
  ul_msg[ul_size++] = humidity & 0xFF;


  PRINTF("senddata....");
  for (i=0; i<ul_size; i++)
  {
    PRINTF("%02X ", ul_msg[i]) ;
  }
  BSP_LED_On( LED_BLUE );

  SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, nbTxRepeatFlag, SFX_FALSE);
  
  BSP_LED_Off( LED_BLUE );

  PRINTF("done\n\r");
#ifdef STDBY_ON  
  LPM_SetOffMode(LPM_APPLI_Id, LPM_Enable);
#else
  /*enable user to press button after transmittion*/
  HW_GPIO_SetIrq( USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, 1, send_data_request );
#endif
}

#ifndef STDBY_ON 
static void send_data_request( void )
{
  /* send task to background*/
  SCH_SetTask( SEND_TASK );
}
#endif

sfx_error_t st_sigfox_open( st_sfx_rc_t SgfxRc )
{
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

      sfx_u32 config_words[3] = {RC2_SET_STD_CONFIG_SM_WORD_0, RC2_SET_STD_CONFIG_SM_WORD_1, RC2_SET_STD_CONFIG_SM_WORD_2 };

      error = SIGFOX_API_open(&SgfxRc.param );

      if ( error == SFX_ERR_NONE )
      {
        error = SIGFOX_API_set_std_config(  config_words, RC2_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    case RC3C_ID:
    {
      sfx_u32 config_words[3] = {0x00000003,0x00001388,0x00000000};

      error = SIGFOX_API_open(&SgfxRc.param );

      if ( error == SFX_ERR_NONE )
      {
        error = SIGFOX_API_set_std_config( config_words, NA);
      }

      break;
    }
    case RC4_ID:
    {
      sfx_u32 config_words[3] = {RC4_SET_STD_CONFIG_SM_WORD_0, RC4_SET_STD_CONFIG_SM_WORD_1, RC4_SET_STD_CONFIG_SM_WORD_2 };

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
  return error;
}

#ifndef STDBY_ON 
/* when STDBY_ON the reset button is used instead of the push button */
static void user_button_init( void )
{

  GPIO_InitTypeDef initStruct={0};

  initStruct.Mode =GPIO_MODE_IT_RISING;
  initStruct.Pull = GPIO_PULLUP;
  initStruct.Speed = GPIO_SPEED_HIGH;
  HW_GPIO_Init( USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, &initStruct );
  
  /* send everytime button is pushed */
  HW_GPIO_SetIrq( USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, 1, send_data_request );
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
