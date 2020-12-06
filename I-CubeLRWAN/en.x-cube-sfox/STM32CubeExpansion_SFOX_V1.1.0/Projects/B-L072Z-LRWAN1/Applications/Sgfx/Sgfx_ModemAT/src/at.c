/*******************************************************************************
 * @file    at.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    30-April-2018
 * @brief   at command API
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
#include "at.h"
#include "utilities.h"
#include "sigfox_api.h"
#include "vcom.h"
#include "tiny_sscanf.h"
#include "version.h"
#include "hw_eeprom.h"
#include "sgfx_sx1276_driver.h"
#include "addon_sigfox_verified_api.h"
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define SGFX_MAX_PAYLOAD_SIZE 12
/**
 * @brief Max size of the data that can be received
 */
#define MAX_RECEIVED_DATA 255
#define SGFX_RX_NB_DATA  16

/* Private macro -------------------------------------------------------------*/
/**
 * @brief Macro to return when an error occurs
 */
#define CHECK_STATUS(status) do {                    \
    ATEerror_t at_status = translate_status(status); \
    if (at_status != AT_OK) { return at_status; }    \
  } while (0)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Print n bytes as %02x
 * @param  the pointer containing the bytes to print
 * @param  n th number of bytes
 * @retval None
 */
static void print_n(uint8_t *pt, uint32_t n);

/**
 * @brief  Print receveid n bytes as %02x
 * @param  the pointer containing the bytes to print
 * @param  n th number of bytes
 * @retval None
 */
static void print_rx(uint8_t *pt, uint32_t n);

/**
 * @brief  Print an unsigned int
 * @param  the value to print
 * @retval None
 */
static void print_u(unsigned int value);


/**
 * @brief  get the length and the number of parameters of the string
 * @param  the string
 * @param  the output vector
 * @param  the length of the output vector
 * @retval None
*/
static ErrorStatus stringToData(const char *str, uint8_t *data, uint32_t* dataSize);
  
  
static ErrorStatus isHex(char Char);

/**
 * @brief  Converts hexa string to a nibble ( 0x0X )
 * @param  hexa string
 * @retval the nibble. Returns 0xF0 in case input was not an hexa string (a..f; A..F or 0..9)
*/
static uint8_t Char2Nibble(char Char);

/**
 * @brief  reopens Sigfox anr rerestores radio configuration as from EEPROM 
 * @param  none
 * @retval none
*/
static void SIGFOX_reopen_and_reconf(void);

/* Exported functions ------------------------------------------------------- */


ATEerror_t AT_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t AT_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t AT_reset(const char *param)
{
  NVIC_SystemReset();
  return AT_OK;
}

ATEerror_t AT_restore_factory_settings(const char *param)
{
  HW_EEPROM_RestoreFs();
  return AT_OK;
}


ATEerror_t AT_SendBit(const char *param)
{
  uint32_t Bit;
  uint32_t dlFlag=0;
  uint32_t txRepeat=1;
  int32_t nbParam;
  uint8_t dl_msg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sfx_error_t error;

  nbParam = tiny_sscanf(param, "%u,%u,%d",&Bit, &dlFlag, &txRepeat);

  if (nbParam >3)
  {
    return AT_PARAM_ERROR;
  }
  
  if (dlFlag ==0)
  { /* no dlFlag */
    if (SIGFOX_API_send_bit(Bit, (sfx_u8 *) dl_msg, txRepeat, SFX_FALSE) != SFX_ERR_NONE)
    {
      return AT_LIB_ERROR;
    }
  }
  else
  { 
    error = SIGFOX_API_send_bit(Bit, (sfx_u8 *) dl_msg, txRepeat, SFX_TRUE);
    
    if ( error == SFX_ERR_NONE)
    {
      print_rx(dl_msg, SGFX_RX_NB_DATA);
    }
    else if (error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      return AT_RX_TIMEOUT;
    }
    else
    {
      return AT_LIB_ERROR;
    }
  }
  return AT_OK;
}

ATEerror_t AT_SendFrame(const char *param)
{
  uint8_t ul_msg[SGFX_MAX_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
  uint8_t dl_msg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t dlFlag=0;     /* default */
  uint32_t  ul_size =0;
  uint32_t nbParam;
  uint32_t txRepeat=1;
  sfx_error_t error;

  /* convert AT param to sgfx param */
  if ( stringToData(param, ul_msg, &ul_size) != SUCCESS )
  {
      return AT_PARAM_ERROR;
  }
  if ( param[2*ul_size] == ',') 
  {
    nbParam = tiny_sscanf( &param[2*ul_size+1],"%u,%u", &dlFlag, &txRepeat );
    
    if (nbParam >2)
    {
      return AT_PARAM_ERROR;
    }
  }

  /*Send Bytes to Sigfox Network */
  if (dlFlag ==0)
  {
    sfx_error_t error= SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_FALSE);
    if ( error == SFX_ERR_NONE)
    {
    }
    else
    {
      return AT_LIB_ERROR;
    }
  }
  else
  {
    error = SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_TRUE);
    
    if ( error == SFX_ERR_NONE)
    {
      print_rx(dl_msg, SGFX_RX_NB_DATA);
    }
    else if (error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      return AT_RX_TIMEOUT;
    }
    else
    {
      return AT_LIB_ERROR;
    }
  }
  
  return AT_OK;
}

ATEerror_t AT_version_get(const char *param)
{
  sfx_u8 *version;
  sfx_u8 size;
  
  SIGFOX_API_get_version( &version, &size, VERSION_SIGFOX);  
  print_n( version, size);
  AT_PRINTF("\r\n");

  SIGFOX_API_get_version( &version, &size, VERSION_RF);  
  print_n( version, size);
  AT_PRINTF("\r\n");
  
  SIGFOX_API_get_version( &version, &size, VERSION_MCU);  
  print_n( version, size);
  AT_PRINTF("\r\n");
 

  SIGFOX_API_get_version( &version, &size, VERSION_SE);  
  print_n( version, size);
  AT_PRINTF("\r\n");

  AT_PRINTF(AT_APP_VERSION"\r\n");
  return AT_OK;
}

ATEerror_t AT_DevPac_get(const char *param)
{
  sfx_u8 SfxPac[8];
  
  sfx_error_t error;
  
  error = SIGFOX_API_get_initial_pac( SfxPac );
  
  if (error != SFX_ERR_NONE)
  {
    return AT_LIB_ERROR;
  }

  for (int i=0; i<8;i++)
  {  
    AT_PRINTF("%02X",SfxPac[i]);
  }
  AT_PRINTF("\r\n");
  return AT_OK;
}

ATEerror_t AT_DevId_get(const char *param)
{
  sfx_u8 SfxId[4];

  sfx_error_t error;
  
  error = SIGFOX_API_get_device_id( SfxId );
  
  if (error != SFX_ERR_NONE)
  {
    return AT_LIB_ERROR;
  }
  
  AT_PRINTF("%02X%02X%02X%02X\r\n", SfxId[3],SfxId[2], SfxId[1], SfxId[0]);
  return AT_OK;
}

ATEerror_t AT_PublicKey_set(const char *param)
{
  if ((param[0] == '0') && ( param[1] == '\0'))
  {
    SE_NVM_set_key_type(CREDENTIALS_KEY_PRIVATE);
  }
  else if ((param[0] == '1') && ( param[1] == '\0'))
  {
    SE_NVM_set_key_type(CREDENTIALS_KEY_PUBLIC);
  }
  else
  {
    return AT_PARAM_ERROR;
  }

 return AT_OK;
}

ATEerror_t AT_PublicKey_get(const char *param)
{

  AT_PRINTF("%d\n\r", (uint8_t) SE_NVM_get_key_type() );

 return AT_OK;
}


ATEerror_t AT_bat_get(const char *param)
{
  print_u(HW_GetBatteryLevel());
  return AT_OK;
}

ATEerror_t AT_test_cw(const char *param)
{
  uint32_t freq=0;
 
  if ( tiny_sscanf(param, "%u",&freq)>1 )
  {
    return AT_PARAM_ERROR;
  }
  
  if (freq == 0)
  {
    SIGFOX_API_stop_continuous_transmission();
    /*reopen after test*/
    SIGFOX_reopen_and_reconf();
  }
  else if ( (freq > ((uint32_t) 100e6)) && (freq < ((uint32_t) 1e9)) )
  {
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq, SFX_NO_MODULATION)!= SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else if ( (freq >  100) && (freq <  1000) )
  { /* user meant Mega... */
    SIGFOX_API_close( );
    if (SIGFOX_API_start_continuous_transmission( freq*1000000 , SFX_NO_MODULATION)!= SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t AT_test_pn(const char *param)
{
  uint32_t freq=0;
  uint32_t bitrate=0;
  sfx_modulation_type_t sfx_bitrate= SFX_DBPSK_100BPS;

  if ( tiny_sscanf(param, "%u,%u",&freq,&bitrate)>2 )
  {
    return AT_PARAM_ERROR;
  }
  
  if ( bitrate == 100 )
  {
    sfx_bitrate = SFX_DBPSK_100BPS;
  }
  else if ( bitrate == 600 )
  {
    sfx_bitrate = SFX_DBPSK_600BPS;
  }
  else if ( bitrate == 0 )
  {
    if (freq != 0)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  
  if (freq == 0)
  {
    SIGFOX_API_stop_continuous_transmission();
    /*reopen after test*/
    SIGFOX_reopen_and_reconf();
  }
  else if ( (freq > ((uint32_t) 100e6)) && (freq < ((uint32_t) 1e9)) )
  {
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq, sfx_bitrate)!= SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else if ( (freq >  100) && (freq <  1000) )
  { /* user meant Mega... */
    SIGFOX_API_close( );
    if (SIGFOX_API_start_continuous_transmission( freq*1000000 , sfx_bitrate)!= SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
}

ATEerror_t AT_test_mode(const char *param)
{
  char test_mode;
  sfx_test_mode_t tm;
  sfx_rc_enum_t rc=SFX_RC1;
  sfx_error_t sfx_error;
  
  ATEerror_t at_status= AT_OK;

  if (( param[0] == '1') && (param[1] == ','))
  {
    rc = SFX_RC1;
    test_mode= param[2];
  }
  else if (( param[0] == '2') && (param[1] == ','))
  {
    rc = SFX_RC2;
    test_mode= param[2];
  }
  else if (( param[0] == '3') && (param[2] == ','))
  {
    if ((param[1] == 'C')||(param[1] == 'c'))
    {
      rc = SFX_RC3C;
      test_mode= param[3];
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else if (( param[0] == '4') && (param[1] == ','))
  {
    rc = SFX_RC4;
    test_mode= param[2];
  }
  else
  {
     return AT_PARAM_ERROR;
  }
  
  if ((test_mode>='0') && (test_mode<='6'))
  {
    tm =(sfx_test_mode_t ) (test_mode - '0');
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  
  SIGFOX_API_close();
  
  sfx_error= ADDON_SIGFOX_VERIFIED_API_test_mode( rc, tm);
  
  /*run command*/ 
  if ( sfx_error!= SFX_ERR_NONE)
  {
    DBG_PRINTF("ERROR %02X", sfx_error);
    at_status= AT_ERROR;
  }
  
  SIGFOX_reopen_and_reconf();
  return at_status;
}



ATEerror_t AT_power_set(const char *param)
{
  int8_t power;
  
  if (tiny_sscanf(param, "%hhd", &power) != 1)
  {
    return AT_PARAM_ERROR;
  }
  
  if (SGFX_SX1276_setPower( power )!= MOD_SUCCESS )
  {
    return AT_PARAM_ERROR;
  }
  
  return AT_OK;
}

ATEerror_t AT_power_get(const char *param)
{
  AT_PRINTF("%d\r\n",SGFX_SX1276_getPower( ) );
  
  return AT_OK;
}

ATEerror_t AT_outOfBand_run(const char *param)
{
  SIGFOX_API_send_outofband( SFX_OOB_SERVICE );
  
  return AT_OK;
}


ATEerror_t AT_ChannelConfigFcc_set(const char *param)
{
  sfx_u32 config_words[3]={0};
  sfx_u16 timer_state;
  int i,j;
  uint8_t nibble;
  st_sfx_rc_t SgfxRc = E2pData.SgfxRc;
  
  /* can change the value only in SFX_FH */
  if ( E2pData.SgfxRc.param.spectrum_access != SFX_FH )
  {
    return AT_PARAM_ERROR;
  }
  
  for (i=0, j=(2*32+28); i<24;i++,j-=4)
  {
    nibble=Char2Nibble(*param++);
    if (nibble<16)
    {
      config_words[i/8] |= ( nibble << (j%32 ) );
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  
  if (tiny_sscanf(param, ",%hu", &timer_state) != 1)
  {
    return AT_PARAM_ERROR;
  }

  if ( SgfxRc.id == RC2_ID )
  {
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc2[0], config_words[0]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc2[1], config_words[1]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc2[2], config_words[2]);
  }
  
  if ( SgfxRc.id == RC3C_ID )
  {
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc3[0], config_words[0]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc3[1], config_words[1]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc3[2], config_words[2]);
  }
  
  if (SgfxRc.id == RC4_ID )
  {
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc4[0], config_words[0]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc4[1], config_words[1]);
    HW_EEPROM_WRITE(E2pData.macroch_config_words_rc4[2], config_words[2]);
  }
  
  if ( (SIGFOX_API_close( )) != SFX_ERR_NONE )
  {
    return AT_ERROR;
  }
  if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
  {
    return AT_ERROR;
  }

  if (SIGFOX_API_set_std_config(config_words, (sfx_bool) timer_state )!= SFX_ERR_NONE )
  {
    return AT_ERROR;
  }

  if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
  {
    return AT_ERROR;
  }
  return AT_OK;
}


ATEerror_t AT_rc_get(const char *param)
{
  e_sfx_rc_id_t rcId = E2pData.SgfxRc.id;
  
  switch (rcId)
  {
    case RC1_ID:
    {
      AT_PRINTF("1\r\n");
      break;
    }
    case RC2_ID:
    {
      AT_PRINTF("2\r\n");
      break;
    }
    case RC3C_ID:
    {
      AT_PRINTF("3C\r\n");
      break;
    }
    case RC4_ID:
    {
      AT_PRINTF("4\r\n");
      break;
    }
    default:
      break;
  }
  return AT_OK;
}
/*AT$RC={1, 2, 3, 4}*/
ATEerror_t AT_rc_set(const char *param)
{
 
  /*record RC*/
  if (param[0] =='1' && param[1]=='\0')
  {
    st_sfx_rc_t SgfxRc= ST_RC1;
    
    HW_EEPROM_WRITE(E2pData.SgfxRc, SgfxRc);
    
    if ( (SIGFOX_API_close( )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    
    SGFX_SX1276_setPower( TX_POWER_14DBM );
    
  }
  else if (param[0] =='2' && param[1]=='\0')
  {
    sfx_u32 config_words[3];

    st_sfx_rc_t SgfxRc= ST_RC2;
    
    config_words[0] = E2pData.macroch_config_words_rc2[0];
    config_words[1] = E2pData.macroch_config_words_rc2[1];
    config_words[2] = E2pData.macroch_config_words_rc2[2];
    
    HW_EEPROM_WRITE(E2pData.SgfxRc, SgfxRc);
    
    if ( (SIGFOX_API_close( )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    
    SGFX_SX1276_setPower( TX_POWER_20DBM );
    
    if (SIGFOX_API_set_std_config(  config_words, RC2_SET_STD_TIMER_ENABLE )!= SFX_ERR_NONE )
    {
       return AT_ERROR;
    }
  }
  else if (param[0] =='3' )
  {
    if (( (param[1] =='c') || (param[1] =='C')) &&(param[2] =='\0'))
    { //RC3C
      sfx_u32 config_words[3];

      st_sfx_rc_t SgfxRc= ST_RC3C;
      
      config_words[0] = E2pData.macroch_config_words_rc3[0];
      config_words[1] = E2pData.macroch_config_words_rc3[1];
      config_words[2] = E2pData.macroch_config_words_rc3[2];
      
      HW_EEPROM_WRITE(E2pData.SgfxRc, SgfxRc);
      
      if ( (SIGFOX_API_close( )) != SFX_ERR_NONE )
      {
        return AT_ERROR;
      }
      if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
      {
        return AT_ERROR;
      }
      
      SGFX_SX1276_setPower( TX_POWER_13DBM );
      
      if (SIGFOX_API_set_std_config(  config_words, NA )!= SFX_ERR_NONE )
      {
         return AT_ERROR;
      }
    }
    else
    {
       return AT_PARAM_ERROR;
    }
  }
  else if (param[0] =='4' && param[1]=='\0')
  {
    sfx_u32 config_words[3];

    st_sfx_rc_t SgfxRc=ST_RC4;    

    config_words[0] = E2pData.macroch_config_words_rc4[0];
    config_words[1] = E2pData.macroch_config_words_rc4[1];
    config_words[2] = E2pData.macroch_config_words_rc4[2];
    
    HW_EEPROM_WRITE(E2pData.SgfxRc, SgfxRc);
    
    if ( (SIGFOX_API_close( )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    if ( (SIGFOX_API_open(&SgfxRc.param )) != SFX_ERR_NONE )
    {
      return AT_ERROR;
    }
    SGFX_SX1276_setPower( TX_POWER_20DBM );
    
    if ( SIGFOX_API_set_std_config( config_words, RC4_SET_STD_TIMER_ENABLE)!= SFX_ERR_NONE )
    {
       return AT_ERROR;
    }
    
    SGFX_SX1276_setPower( TX_POWER_20DBM );
  }
  else
  { 
    return AT_PARAM_ERROR;
  }

  return AT_OK;
}


ATEerror_t AT_rssi_cal_set(const char *param)
{
  int16_t rssi_cal;
  if (tiny_sscanf(param, "%hd",&rssi_cal) != 1)
  {
    return AT_PARAM_ERROR;
  }
  
    HW_EEPROM_WRITE(E2pData.rssi_cal, rssi_cal);
  
  return AT_OK;
}

ATEerror_t AT_rssi_cal_get(const char *param)
{

  AT_PRINTF("%d dB\r\n",E2pData.rssi_cal);

  return AT_OK;
}
ATEerror_t AT_echo_set(const char *param)
{
  uint32_t echoState;
  if (tiny_sscanf(param, "%u",&echoState) != 1)
  {
    return AT_PARAM_ERROR;
  }
  
  if (echoState == 0)
  {
    HW_EEPROM_WRITE(E2pData.AtEcho, E2P_RST);
  }
  else if (echoState == 1)
  {
    HW_EEPROM_WRITE(E2pData.AtEcho, E2P_SET);
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  return AT_OK;
}

ATEerror_t AT_echo_get(const char *param)
{
  if (E2pData.AtEcho ==E2P_SET)
  {
    AT_PRINTF("1\r\n");
  }
  else
  {
     AT_PRINTF("0\r\n");
  }
  return AT_OK;
}

/* Private functions ---------------------------------------------------------*/

static void print_rx(uint8_t *pt, uint32_t n)
{
  AT_PRINTF("\r\n+RX=");
  for (int i =0; i<n; i++)
  {
    AT_PRINTF("%c",pt[i]);
  }
  AT_PRINTF("\r\n");
}

static void print_n(uint8_t *pt, uint32_t n)
{
  for (int i =0; i<n; i++)
    AT_PRINTF("%c",pt[i]);
}

static void print_u(unsigned int value)
{
  AT_PRINTF("%u\r\n", value);
}

static ErrorStatus stringToData(const char *str, uint8_t *data, uint32_t* dataSize)
{
  uint8_t ii =0;
  char hex[3];
  hex[2] = 0;
  while (( *str != '\0') && ( *str != ','))
  {
    hex[0] = *str++;
    hex[1] = *str++;
    
    /*check if input is hexa */
    if ( (isHex(hex[0])==ERROR) ||  (isHex(hex[1])==ERROR) )
    {
      return ERROR;
    }
     /*check if input is even nb of character*/
    if ( (hex[1] == '\0') || ( hex[1] == ',') )
    {
      return ERROR;
    }
    if (tiny_sscanf(hex, "%hhx", &data[ii]) != 1)
    {
      return ERROR;
    }
    ii++;
    if (ii > SGFX_MAX_PAYLOAD_SIZE)
    {
      return ERROR;
    }
  }
  *dataSize = ii;
  if (ii==0)
  {
    return ERROR;
  }
  return SUCCESS;
}
static ErrorStatus isHex(char Char)
{
  if ( ((Char >='0') && (Char <='9')) || 
       ((Char >='a') && (Char <='f')) ||
       ((Char >='A') && (Char <='F')) )
  {
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
}

static uint8_t Char2Nibble(char Char)
{
  if ( ((Char >='0') && (Char <='9')))
  {
    return Char-'0';
  }
  else if ( ((Char >='a') && (Char <='f')) )
  {
     return Char-'a'+10;
  }
  else if ((Char >='A') && (Char <='F')) 
  {
     return Char-'A'+10;
  }
  else
  {
    return 0xF0;
  }
}

static void SIGFOX_reopen_and_reconf(void)
{
    SIGFOX_API_open(&E2pData.SgfxRc.param );
    if(E2pData.SgfxRc.id == RC2_ID)
    {
      SIGFOX_API_set_std_config( E2pData.macroch_config_words_rc2, RC2_SET_STD_TIMER_ENABLE);
    }
    if(E2pData.SgfxRc.id == RC3C_ID)
    {
      SIGFOX_API_set_std_config( E2pData.macroch_config_words_rc3, NA);
    }
    if(E2pData.SgfxRc.id == RC4_ID)
    {
      SIGFOX_API_set_std_config( E2pData.macroch_config_words_rc4, RC4_SET_STD_TIMER_ENABLE);
    } 
}
                                
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

