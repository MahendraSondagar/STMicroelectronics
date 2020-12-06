/**
  ******************************************************************************
  * @file    SENSING1.h
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   FP-AI-SENSING1 global defines
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
#ifndef __SENSING1_H
#define __SENSING1_H

#include "SENSING1_config.h"

/* Exported define ------------------------------------------------------------*/

/**************************************
 *  Lab/Experimental section defines  *
***************************************/

/* For enabling connection and notification subscriptions debug */
//#define SENSING1_DEBUG_CONNECTION

/* Define the SENSING1 MAC address, otherwise it will create a Unique MAC */
//#define MAC_SENSING1 0xFF, 0xEE, 0xAA, 0xAA, 0xAA, 0xAA

#ifndef MAC_SENSING1
/* For creating one MAC related to STM32 UID, Otherwise the BLE will use it's random MAC */
  //#define MAC_STM32UID_SENSING1
#endif /* MAC_SENSING1 */

/* For HAR_IGN_WSDM play back mode (use test vectors instead of sensors) */
#define TEST_IGN_WSDM

/* Uncomment the following define for changing the default BLE Advertise Interval
 * This define will reduce the Power Consumption but will increase the board discovery section.
 * It's necessary to use the latest Android/iOS application. */
#ifndef SENSING1_BlueNRG2 
  /* For BlueNRG-1 must be defined */
  #define BLE_CHANGE_ADV_INTERVAL
#endif /* SENSING1_BlueNRG2 */

/* Uncomment the following define for forcing a full BLE rescan for the Android/iOS "ST BLE Sensor" application*/
#define BLE_FORCE_RESCAN

/* Uncomment the following define for reading the Environmental sensors with a Single shot 
 * modality instead of Continuous mode */
#define ONE_SHOT

//#define BLE_LINK_ADAPT

/***********************************************************
 *  Extend defines                                         *
 ***********************************************************/

/**
 * @brief Use USB Communications Device Class
 */
#if (defined(STM32_SENSORTILE) && SENSING1_USE_PRINTF)
#define SENSING1_USE_USB_CDC      1
#else
#define SENSING1_USE_USB_CDC      0
#endif /* (defined(STM32_SENSORTILE) && SENSING1_USE_PRINTF) */

/**
 * @brief Use USB Mass Storage Device Class
 *        When enabled, the local filesystem can be accessed using the USB OTG
 *        connector.
 */
#if (defined(USE_STM32L475E_IOT01) && SENSING1_USE_DATALOG)
#define SENSING1_USE_USB_MSC      1
#else
#define SENSING1_USE_USB_MSC      0
#endif /* (defined(USE_STM32L475E_IOT01) && SENSING1_USE_DATALOG) */

#if (SENSING1_USE_USB_AUDIO || SENSING1_USE_USB_MSC || SENSING1_USE_USB_CDC)
#define SENSING1_USE_USB         1
#else
#define SENSING1_USE_USB         0
#endif

/**
 * @brief Battery powered platform
 *        Allows for battery information reporting.
 */
#if (defined(STM32_SENSORTILE) || defined(STM32_SENSORTILEBOX))
#define SENSING1_USE_BATTERY      1
#else
#define SENSING1_USE_BATTERY      0
#endif

/***********************************************************
 * Check for allowed defines configurations (don't change) *
 ***********************************************************/

#if SENSING1_USE_USB_AUDIO
#if (SENSING1_USE_USB_MSC || SENSING1_USE_USB_CDC)
#error "USB composite is not supported. Only one USB Class can be defined."
#endif /* (SENSING1_USE_USB_MSC || SENSING1_USE_USB_CDC) */
#endif /* SENSING1_USE_USB_AUDIO */

#if SENSING1_USE_USB_CDC
#if (SENSING1_USE_USB_MSC || SENSING1_USE_USB_AUDIO)
#error "USB composite is not supported. Only one USB Class can be defined."
#endif /* (SENSING1_USE_USB_MSC || SENSING1_USE_USB_AUDIO) */
#endif /* SENSING1_USE_USB_CDC */

#if SENSING1_USE_USB_MSC
#if (SENSING1_USE_USB_CDC || SENSING1_USE_USB_AUDIO)
#error "USB composite is not supported. Only one USB Class can be defined."
#endif /* (SENSING1_USE_USB_CDC || SENSING1_USE_USB_AUDIO) */
#endif /* SENSING1_USE_USB_MSC */

#if SENSING1_USE_BATTERY
#if (defined(USE_STM32L4XX_NUCLEO) || defined(USE_STM32L475E_IOT01))
#error "Platform is not battery powered."
#endif /* (defined(USE_STM32L4XX_NUCLEO) || defined(USE_STM32L475E_IOT01)) */
#endif /* SENSING1_USE_BATTERY */

#if SENSING1_USE_DATALOG
#if defined(USE_STM32L4XX_NUCLEO)
#error "Platform does not support the datalog feature."
#endif /* defined(USE_STM32L4XX_NUCLEO) */
#endif /* SENSING1_USE_DATALOG */

/**************************************
 * Don't Change the following defines *
***************************************/

/* Package Version only numbers 0->9 */
#define SENSING1_VERSION_MAJOR '4'
#define SENSING1_VERSION_MINOR '0'
#define SENSING1_VERSION_PATCH '0'

/* Define The transmission interval [mSec] for Microphones dB Values */
#define MICS_DB_UPDATE_MS 50

/* Define The transmission interval [mSec] for Environmental Measures and Battery Informations */
#define ENV_UPDATE_MS 500

/* Define The transmission interval [mSec] for Inertial Measures */
#define INERTIAL_UPDATE_MS 50

/* Define Inertial Acquisition interval [mSec] for Activity Recognition */
#define INERTIAL_ACQ_ACTIVITY_GMP_HZ      (26.0F)
#define INERTIAL_ACQ_ACTIVITY_IGN_HZ      (26.0F)
#define INERTIAL_ACQ_ACTIVITY_IGN_WSDM_HZ (20.0F)
#define INERTIAL_ACQ_ACTIVITY_GMP_MS      (38) /*26Hz*/
#define INERTIAL_ACQ_ACTIVITY_IGN_MS      (38) /*26Hz*/
#define INERTIAL_ACQ_ACTIVITY_IGN_WSDM_MS (50) /*20Hz*/
#define HAR_GMP_ALG_ID                    (1)
#define HAR_IGN_ALG_ID                    (1)
#define HAR_IGN_WSDM_ALG_ID               (2)

/* Define the SENSING1 Name MUST be 7 char long */
#if defined(STM32_SENSORTILE)
  #define NAME_BLUEMS 'T','A','I','_',SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH  
#elif defined(USE_STM32L4XX_NUCLEO)
  #define NAME_BLUEMS 'N','A','I','_',SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH
#elif defined(USE_STM32L475E_IOT01)
  #define NAME_BLUEMS 'I','A','I','_',SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH
#elif defined(STM32_SENSORTILEBOX)
  #define NAME_BLUEMS 'B','A','I','_',SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH
#else
#error "Write Something here"
#endif /* STM32_SENSORTILE */

/* Package Name */
#define SENSING1_PACKAGENAME "AI-SENSING1"

#if defined(USE_STM32L4XX_NUCLEO)
  #define AUDIO_VOLUME_VALUE       64
  /* for having the AUDIO_SAMPLING_FREQUENCY */
  #include "x_nucleo_cca02m1_conf.h"
#elif defined(STM32_SENSORTILE)
 /* for having the AUDIO_SAMPLING_FREQUENCY */
  #include "SensorTile_conf.h"
  #define AUDIO_VOLUME_VALUE       32
#elif defined(USE_STM32L475E_IOT01)
 /* for having the AUDIO_SAMPLING_FREQUENCY */
  #include "stm32l475e_iot01_conf.h"
  #define AUDIO_VOLUME_VALUE       32
#elif defined(STM32_SENSORTILEBOX)
  /* for having the AUDIO_SAMPLING_FREQUENCY */
  #include "SensorTile.box_conf.h"
  #define AUDIO_VOLUME_VALUE       32
  #define AUDIO_CHANNELS           1
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */
  #define PCM_AUDIO_IN_SAMPLES     (AUDIO_SAMPLING_FREQUENCY / 1000)

#if SENSING1_USE_PRINTF
  extern uint8_t Sensing1PrintfEnabled;
  #define SENSING1_PRINTF(...)       do {if (Sensing1PrintfEnabled) {printf(__VA_ARGS__);}}while ( 0 )
  #define _SENSING1_PRINTF(...)    printf(__VA_ARGS__);
  #define SENSING1_PRINTF_FLUSH()  fflush(stdout)
#else /* SENSING1_USE_PRINTF */
  #define SENSING1_PRINTF(...)
  #define _SENSING1_PRINTF(...)
  #define SENSING1_PRINTF_FLUSH()
#endif /* SENSING1_USE_PRINTF */

#if SENSING1_USE_PWR_MGNT
  #define SENSING1_USE_CLI 0
#else
  #if SENSING1_USE_PRINTF 
    #define SENSING1_USE_CLI 1
  #else
    #define SENSING1_USE_CLI 0
  #endif
#endif

/* Remapping Section */
#include "MEMS_FuncRemapping.h"

#endif /* __SENSING1_H */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
