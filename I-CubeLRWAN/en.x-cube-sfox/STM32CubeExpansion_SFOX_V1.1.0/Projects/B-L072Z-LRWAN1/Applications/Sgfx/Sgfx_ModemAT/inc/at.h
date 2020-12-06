/*******************************************************************************
 * @file    at.h
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    30-April-2018
 * @brief   Header for driver at.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT_H__
#define __AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/*
 * AT Command Id errors. Note that they are in sync with ATError_description static array
 * in command.c
 */
typedef enum eATEerror
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_LIB_ERROR,
  AT_RX_TIMEOUT,
  AT_RX_ERROR,
  AT_MAX,
} ATEerror_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* AT printf */
#define AT_PRINTF(...)     vcom_Send(__VA_ARGS__)

/* AT Command strings. Commands start with AT */
#define AT_RESET      "Z"
#define AT_RFS        "$RFS"
#define AT_VER        "+VER"
#define AT_ID         "$ID"
#define AT_PAC        "$PAC"
#define AT_S410       "S410"
#define AT_SENDB      "$SB"
#define AT_SENDF      "$SF"
#define AT_TM         "$TM"
#define AT_CW         "$CW"
#define AT_PN         "$PN"
#define AT_BAT        "+BAT"
#define AT_S302       "S302"
#define AT_S300       "S300"
#define AT_S400       "S400"
#define AT_RC         "$RC"
#define AT_RSSICAL    "$RSSICAL"
#define ATE           "E"

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Return AT_OK in all cases
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t AT_return_ok(const char *param);

/**
 * @brief  Return AT_ERROR in all cases
 * @param  Param string of the AT command - unused
 * @retval AT_ERROR
 */
ATEerror_t AT_return_error(const char *param);

/**
 * @brief  Trig a reset of the MCU
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t AT_reset(const char *param);

/**
 * @brief  Restore factory settings in Eeprom
 * @param  Param string of the AT command - unused
 * @retval AT_OK
 */
ATEerror_t AT_restore_factory_settings(const char *param);

/**
 * @brief  Send Bit w/wo ack to Sigfox Nw
 * @param  String pointing to provided ADR setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t AT_SendBit(const char *param);

/**
 * @brief  Send  frame w/wo ack to Sigfox Nw
 * @param  String pointing to provided ADR setting
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t AT_SendFrame(const char *param);

/**
 * @brief  Print last received message
 * @param  String parameter
 * @retval AT_OK if OK, or an appropriate AT_xxx error code
 */
ATEerror_t AT_Send(const char *param);

/**
 * @brief  Get the DevId 
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_DevId_get(const char *param);
  
/**
 * @brief  Get the Dev Pac 
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_DevPac_get(const char *param);

/**
 * @brief  Set public Key
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_PublicKey_set(const char *param);

/**
 * @brief  Get public Key
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_PublicKey_get(const char *param);
/**
 * @brief  Print the version of the AT_Slave FW
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_version_get(const char *param);

/**
 * @brief  Print the batery level
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_bat_get(const char *param);
  
/**
 * @brief  Test Tone
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_test_cw(const char *param);

/**
 * @brief  Tx Test with prbs9 modulation
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_test_pn(const char *param);

  /**
 * @brief  Test Tone
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_test_mode(const char *param);
  
/**
 * @brief  set the output power of the radio (power in dBm)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_power_set(const char *param);

/**
 * @brief  get the output power of the radio (power in dBm)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_power_get(const char *param);
/**
 * @brief  send an out of band message one
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_outOfBand_run(const char *param);

/**
 * @brief  to configure the enabled channels for FCC
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_ChannelConfigFcc_set(const char *param);

/**
 * @brief  set zones (1 2 3 or 4)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_rc_set(const char *param);

/**
 * @brief  get zones (1 2 3 or 4)
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_rc_get(const char *param);

/**
 * @brief  to get the rssi calibration value from eeprom.
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_rssi_cal_get(const char *param);
  
/**
 * @brief  to set the rssi calibration state in eeprom.
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_rssi_cal_set(const char *param);
/**
 * @brief  to get the current echo state from eeprom.
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_echo_get(const char *param);
  
/**
 * @brief  to set the echo state in eeprom.
 * @param  String parameter
 * @retval AT_OK
 */
ATEerror_t AT_echo_set(const char *param);
  
#ifdef __cplusplus
}
#endif

#endif /* __AT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

