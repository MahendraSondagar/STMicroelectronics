/*******************************************************************************
 * @file    command.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    30-April-2018
 * @brief   main command driver dedicated to command AT
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
#include <stdlib.h>
#include "at.h"
#include "hw.h"
#include "command.h"

/* comment the following to have help message */
/* #define NO_HELP */
/* #define NO_KEY_ADDR_EUI */

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief  Structure defining an AT Command
 */
struct ATCommand_s {
  const char *string;                       /*< command string, after the "AT" */
  const int size_string;                    /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param);     /*< ? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
#if !defined(NO_HELP)
  const char *help_string;                  /*< to be printed when AT? */
  const char *help_param;                  /*< to be printed when =? after the string */
#endif
};

/* Private define ------------------------------------------------------------*/
#define CMD_SIZE 64

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief  Array corresponding to the description of each possible AT Error
 */
static const char *const ATError_description[] =
{
  "\r\nOK\r\n",                     /* AT_OK */
  "\r\nAT_ERROR\r\n",               /* AT_ERROR */
  "\r\nAT_PARAM_ERROR\r\n",         /* AT_PARAM_ERROR */
  "\r\nAT_BUSY_ERROR\r\n",          /* AT_BUSY_ERROR */
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n", /* AT_TEST_PARAM_OVERFLOW */
  "\r\nAT_LIB_ERROR\r\n",   /* AT_LIB_ERROR */
  "\r\nAT_RX_TIMEOUT\r\n",            /* AT_RX_TIMEOUT */
  "\r\nAT_RX_ERROR\r\n",            /* AT_RX_ERROR */
  "\r\nerror unknown\r\n",          /* AT_MAX */
};

/**
 * @brief  Array of all supported AT Commands
 */
static const struct ATCommand_s ATCommand[] =
{
  {
    .string = AT_RESET,
    .size_string = sizeof(AT_RESET) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RESET ": Trig a MCU reset\r\n",
    .help_param = "",
#endif
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_reset,
  },

  {
    .string = AT_RFS,
    .size_string = sizeof(AT_RFS) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RFS ": Restore EEPROM Factory Settings\r\n",
    .help_param = "",
#endif
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_restore_factory_settings,
  },
  
  {
    .string = AT_VER,
    .size_string = sizeof(AT_VER) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_VER ": Get the FW version\r\n",
    .help_param = "",
#endif
    .get = AT_version_get,
    .set = AT_return_error,
    .run = AT_version_get,
  },

  {
    .string = AT_ID,
    .size_string = sizeof(AT_ID) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_ID ": Get the ID\r\n",
    .help_param = "",
#endif
    .get = AT_DevId_get,
    .set = AT_return_error,
    .run = AT_DevId_get,
  },

  {
    .string = AT_PAC,
    .size_string = sizeof(AT_PAC) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PAC ": Get the PAC\r\n",
    .help_param = "",
#endif
    .get = AT_DevPac_get,
    .set = AT_return_error,
    .run = AT_DevPac_get,
  },

  {
    .string = AT_S410,
    .size_string = sizeof(AT_S410) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S410 ": 0:Private Key 1:Public Key\r\n",
    .help_param = "[0 ..1]",
#endif
    .get = AT_PublicKey_get,
    .set = AT_PublicKey_set,
    .run = AT_return_error,
  },

  {
    .string = AT_SENDB,
    .size_string = sizeof(AT_SENDB) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDB ": Send a bit to the sigfox network\r\n",
    .help_param = "AT"AT_SENDB"=<Bit>,<Opt downlink>,<Opt TxRepeat><CR>",
#endif
    .get = AT_return_error,
    .set = AT_SendBit,
    .run = AT_return_error,
  },
  
  {
    .string = AT_SENDF,
    .size_string = sizeof(AT_SENDF) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_SENDF ": Send a frame to the sigfox network\r\n",
    .help_param = "AT"AT_SENDF"=<Payload>,<Opt downlink>,<Opt TxRepeat><CR>",
#endif
    .get = AT_return_error,
    .set = AT_SendFrame,
    .run = AT_return_error,
  },

  {
    .string = AT_CW,
    .size_string = sizeof(AT_CW) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_CW ": Run CW Test mode\r\n",
    .help_param = "AT"AT_CW"=<frequency><CR> frequency in Hz or in MHz,",
#endif
    .get = AT_return_error,
    .set = AT_test_cw,
    .run = AT_return_error,
  },
  
  {
    .string = AT_PN,
    .size_string = sizeof(AT_PN) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_PN ": Run PRBS9 BPBSK Test mode\r\n",
    .help_param = "AT"AT_PN"=<frequency>,<bitrate><CR> frequency in Hz or in MHz, bitrate=[100, 600] ",
#endif
    .get = AT_return_error,
    .set = AT_test_pn,
    .run = AT_return_error,
  },
  
  {
    .string = AT_TM,
    .size_string = sizeof(AT_TM) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_TM ": Run Sigfox Test mode\r\n",
    .help_param = "AT"AT_TM"=<rc>,<mode><CR> ",
#endif
    .get = AT_return_error,
    .set = AT_test_mode,
    .run = AT_return_error,
  },
  
  {
    .string = AT_BAT,
    .size_string = sizeof(AT_BAT) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_BAT ": Get the battery Level in mV \r\n",
    .help_param = "",
#endif
    .get = AT_bat_get,
    .set = AT_return_error,
    .run = AT_return_error,
  },

  {
    .string = AT_S302,
    .size_string = sizeof(AT_S302) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S302 ": set Radio output power in dBm\r\n",
    .help_param = "AT"AT_S302"=<power>,<CR>  power in dBm=[0..20]", 
#endif
    .get = AT_power_get,
    .set = AT_power_set,
    .run = AT_return_error,
  },

  {
    .string = AT_S300,
    .size_string = sizeof(AT_S300) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S300 ": send an out of band message once.\r\n",
    .help_param = "",
#endif
    .get = AT_return_error,
    .set = AT_return_error,
    .run = AT_outOfBand_run,
  },

  {
    .string = AT_S400,
    .size_string = sizeof(AT_S400) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_S400 ": configure specific variables for standard.\r\n",
    .help_param = "<param1><param2><param3>,<param4><CR>",
#endif
    .get = AT_return_error,
    .set = AT_ChannelConfigFcc_set,
    .run = AT_return_error,
  },

  {
    .string = AT_RC,
    .size_string = sizeof(AT_RC) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RC ": to set/get the Regional Config zones.\r\n",
    .help_param = "<param1><CR> param1=[1,2,3,4]",
#endif
    .get = AT_rc_get,
    .set = AT_rc_set,
    .run = AT_return_error,
  },
  
  {
    .string = AT_RSSICAL,
    .size_string = sizeof(AT_RSSICAL) - 1,
#ifndef NO_HELP
    .help_string = "AT"AT_RSSICAL ": to set/get the rssi calibration value in dB.\r\n",
    .help_param = "<param1><CR> param1=rssi value in dB",
#endif
    .get = AT_rssi_cal_get,
    .set = AT_rssi_cal_set,
    .run = AT_return_error,
  }, 

  {
    .string = ATE,
    .size_string = sizeof(ATE) - 1,
#ifndef NO_HELP
    .help_string = "AT"ATE ": to set/get the echo state.\r\n",
    .help_param = "<param1><CR> param1=[0,1]",
#endif
    .get = AT_echo_get,
    .set = AT_echo_set,
    .run = AT_return_error,
  },  
};


/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Print a string corresponding to an ATEerror_t
 * @param  The AT error code
 * @retval None
 */
static void com_error(ATEerror_t error_type);

/**
 * @brief  Parse a command and process it
 * @param  The command
 * @retval None
 */
static void parse_cmd(const char *cmd);

/* Exported functions ---------------------------------------------------------*/

void CMD_Init(void)
{
  vcom_Init();
  vcom_ReceiveInit();
}

void CMD_Process(void)
{
  static char command[CMD_SIZE];
  static unsigned i = 0;
  unsigned cmd_size = 0;

  /* Process all commands */
  while (IsNewCharReceived() == SET)
  {
    command[i] = GetNewChar();

#if 0 /* echo On    */
    PRINTF("%c", command[i]);
#endif

    if (command[i] == AT_ERROR_RX_CHAR)
    {
      memset(command, '\0', i);
      i = 0;
      com_error(AT_RX_ERROR);
      break;
    }
    else
    if ((command[i] == '\r') || (command[i] == '\n'))
    {
      if (i != 0)
      {
        command[i] = '\0';
        /* need to put static i=0 to avoid realtime issue when CR+LF is used by hyperterminal */
        cmd_size = i; 
        i = 0;
        parse_cmd(command);
        
        memset(command, '\0', cmd_size);

      }
    }
    else if (i == (CMD_SIZE - 1))
    {
      memset(command, '\0', i);
      i = 0;
      com_error(AT_TEST_PARAM_OVERFLOW);
    }
    else
    {
      i++;
    }
  }
}

/* Private functions ---------------------------------------------------------*/

static void com_error(ATEerror_t error_type)
{
  if (error_type > AT_MAX)
  {
    error_type = AT_MAX;
  }
  AT_PRINTF(ATError_description[error_type]);
}


static void parse_cmd(const char *cmd)
{
  ATEerror_t status = AT_OK;
  const struct ATCommand_s *Current_ATCommand;
  int i;

  if ((cmd[0] != 'A') || (cmd[1] != 'T'))
  {
    status = AT_ERROR;
  }
  else
  if (cmd[2] == '\0')
  {
    /* status = AT_OK; */
  }
  else
  if (cmd[2] == '?')
  {
#ifdef NO_HELP
#else
    AT_PRINTF("AT?              : Help on <CMD>\r\n");
    AT_PRINTF("AT+<CMD>         : Run the <CMD>\r\n");
    AT_PRINTF("AT+<CMD>?        : Get the value\r\n");
    AT_PRINTF("AT+<CMD>=<value> : Set the value\r\n");
    AT_PRINTF("AT+<CMD>=?       : Get the value range\r\n");
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      AT_PRINTF(ATCommand[i].help_string);
    }
#endif
  }
  else
  {
    /* point to the start of the command, excluding AT */
    status = AT_ERROR;
    cmd += 2;
    for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
    {
      if (strncmp(cmd, ATCommand[i].string, ATCommand[i].size_string) == 0)
      {
        Current_ATCommand = &(ATCommand[i]);
        /* point to the string after the command to parse it */
        cmd += Current_ATCommand->size_string;
        
        /* parse after the command */
        switch (cmd[0])
        {
          case '\0':    /* ATXXX nothing after the command */
            status = Current_ATCommand->run(cmd);
            break;
          case '=':  /* ATXXX=? */
            if ((cmd[1] == '?') && (cmd[2] == '\0'))
            { 
              AT_PRINTF(Current_ATCommand->help_param);
              status = AT_OK;
            }
            else
            { /* ATXXX=input*/
              status = Current_ATCommand->set(cmd+1);
            }
            break;
          case '?':/* ATXXX? */
            status = Current_ATCommand->get(cmd);
            break;
          default:
            /* not recognized */
            break;
        }
        /* we end the loop as the command was found */
        break;
      }
    }
  }

  com_error(status);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
