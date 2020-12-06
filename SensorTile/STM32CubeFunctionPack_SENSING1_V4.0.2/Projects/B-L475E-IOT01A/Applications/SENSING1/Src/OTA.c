/**
  ******************************************************************************
  * @file    OTA.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Over-the-Air Update API implementation
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SENSING1.h"
#include "TargetFeatures.h"

#include "OTA.h"
#include "main.h"

#include "ai_common.h"

/* Local types ---------------------------------------------------------------*/
typedef struct
{
  uint32_t Version;
  uint32_t MagicNum;
  uint32_t OTAStartAdd;
  uint32_t OTADoneAdd;
  uint32_t OTAMaxSize;
  uint32_t ProgStartAdd;
} BootLoaderFeatures_t;

/* Local defines -------------------------------------------------------------*/

#ifndef STM32_SENSORTILEBOX

  /* Board/BlueNRG FW OTA Position */
  #define OTA_ADDRESS_START  0x08080010

  /* Board  FW OTA Magic Number Position */
  #define OTA_MAGIC_NUM_POS  0x08080000

  /* Board  FW OTA DONE Magic Number Position */
  #define OTA_MAGIC_DONE_NUM_POS  0x08080008

#else /* STM32_SENSORTILEBOX */

  /* Board/BlueNRG FW OTA Position */
  #define OTA_ADDRESS_START  0x08100010

  /* Board  FW OTA Magic Number Position */
  #define OTA_MAGIC_NUM_POS  0x08100000

  /* Board  FW OTA DONE Magic Number Position */
  #define OTA_MAGIC_DONE_NUM_POS  0x08100008

#endif /* STM32_SENSORTILEBOX */

/* Board  FW OTA Magic Number */
#define OTA_MAGIC_NUM 0xDEADBEEF

/* Board Partial OTA for NN Weights */
#define NN_OTA_MAGIC_NUM 0xABADBABE

/* Uncomment the following define for enabling the PRINTF capability if it's supported */
#define OTA_ENABLE_PRINTF

#ifdef OTA_ENABLE_PRINTF
  #define OTA_PRINTF _SENSING1_PRINTF
#else /* OTA_ENABLE_PRINTF */
  #define OTA_PRINTF(...)
#endif /* OTA_ENABLE_PRINTF */

/* Local Macros -------------------------------------------------------------*/
#define OTA_ERROR_FUNCTION() { while(1);}

/* Private variables ---------------------------------------------------------*/
static uint32_t SizeOfUpdateBlueFW=0;
static uint32_t ExpecteduwCRCValue=0;
static uint32_t WritingAddress = OTA_ADDRESS_START;

static BootLoaderFeatures_t *BootLoaderFeatures = (BootLoaderFeatures_t *)0x08003F00;

/* Exported functions  --------------------------------------------------*/
/**
 * @brief Function for Testing the BootLoader Compliance
 * @param None
 * @retval int8_t Return value for checking purpose (0/1 == Ok/Error)
 */
int8_t CheckBootLoaderCompliance(void)
{
  OTA_PRINTF("Testing BootLoaderCompliance:\r\n");
  OTA_PRINTF("\tVersion  %u.%u.%u\r\n",
             (unsigned int)BootLoaderFeatures->Version >> 16,
             (unsigned int)(BootLoaderFeatures->Version >> 8) & 0xFF,
             (unsigned int)BootLoaderFeatures->Version & 0xFF);

  if(BootLoaderFeatures->MagicNum==OTA_MAGIC_NUM) {
    OTA_PRINTF("\tMagicNum    OK\r\n");
  } else {
    OTA_PRINTF("\tMagicNum    KO\r\n");
    return 0;
  }

  OTA_PRINTF("\tMaxSize = %u\r\n", (unsigned int) BootLoaderFeatures->OTAMaxSize);

  if(BootLoaderFeatures->OTAStartAdd==(OTA_ADDRESS_START-16)) {
    OTA_PRINTF("\tOTAStartAdd OK\r\n");
  } else {
    OTA_PRINTF("\tOTAStartAdd KO\r\n");
    return 0;
  }

  if(BootLoaderFeatures->OTADoneAdd==OTA_MAGIC_DONE_NUM_POS) {
    OTA_PRINTF("\tOTADoneAdd  OK\r\n");
  } else {
    OTA_PRINTF("\tOTADoneAdd  KO\r\n");
    return 0;
  }

  return 1;
}

/**
 * @brief Function for Updating the Firmware
 * @param uint32_t *SizeOfUpdate Remaining size of the firmware image [bytes]
 * @param uint8_t *att_data attribute data
 * @param int32_t data_length length of the data
 * @param uint8_t WriteMagicNum 1/0 for writing or not the magic number
 * @retval int8_t Return value for checking purpose (1/-1 == Ok/Error)
 */
int8_t UpdateFWBlueMS(uint32_t *SizeOfUpdate, uint8_t *att_data, int32_t data_length, uint8_t WriteMagicNum)
{
  int8_t ReturnValue=0;
  /* Save the Packed received */

  if(data_length>(*SizeOfUpdate)){
    /* Too many bytes...Something wrong... necessity to send it again... */
    OTA_PRINTF("OTA something wrong data_length=%d RemSizeOfUpdate=%u....\r\nPlease Try again\r\n",
    (int) data_length, (unsigned int)(*SizeOfUpdate));
    ReturnValue = -1;
    /* Reset for Restarting again */
    *SizeOfUpdate=0;
  } else {
    uint64_t ValueToWrite;
    int32_t Counter;
    /* Save the received OTA packed ad save it to flash */
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    for(Counter=0;Counter<data_length;Counter+=8) {
      memcpy((uint8_t*) &ValueToWrite,att_data+Counter,data_length-Counter+1);

      if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WritingAddress,ValueToWrite)==HAL_OK) {
       WritingAddress+=8;
      } else {
        /* Error occurred while writing data in Flash memory.
           User can add here some code to deal with this error
           FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
        OTA_ERROR_FUNCTION();
      }
    }
    /* Reduce the remaining bytes for OTA completion */
    *SizeOfUpdate -= data_length;

    if(*SizeOfUpdate==0) {
      /* We had received the whole firmware and we have saved it in Flash */
      OTA_PRINTF("OTA Update saved\r\n");

      if(WriteMagicNum) {
        uint32_t uwCRCValue = 0;

        if(ExpecteduwCRCValue) {
          /* Make the CRC integrity check */
          /* CRC handler declaration */
          CRC_HandleTypeDef   CrcHandle;

          /* Init CRC for OTA-integrity check */
          CrcHandle.Instance = CRC;
          /* The default polynomial is used */
          CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;

          /* The default init value is used */
          CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;

          /* The input data are not inverted */
          CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;

          /* The output data are not inverted */
          CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

          /* The input data are 32-bit long words */
          CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;

          if(HAL_CRC_GetState(&CrcHandle) != HAL_CRC_STATE_RESET) {
            HAL_CRC_DeInit(&CrcHandle);
          }

          if (HAL_CRC_Init(&CrcHandle) != HAL_OK) {
            /* Initialization Error */
            OTA_ERROR_FUNCTION();
          } else {
            OTA_PRINTF("CRC  Initialized\n\r");
          }
          /* Compute the CRC */
          uwCRCValue = HAL_CRC_Calculate(&CrcHandle, (uint32_t *)OTA_ADDRESS_START, SizeOfUpdateBlueFW>>2);

          if(uwCRCValue==ExpecteduwCRCValue) {
            ReturnValue=1;
            OTA_PRINTF("OTA CRC-checked\r\n");
          } else {
            OTA_PRINTF("OTA Error CRC-checking\r\n");
          }
        } else {
          ReturnValue=1;
        }
        if(ReturnValue==1) {
          /* Full/Partial Firmware Update */
          uint32_t HeaderSize;
          uint32_t DestinationAddress;
          uint32_t HeaderAddress = OTA_ADDRESS_START;
          uint32_t Value32;

          WritingAddress = OTA_MAGIC_NUM_POS;
          ValueToWrite=(((uint64_t)SizeOfUpdateBlueFW)<<32)| (OTA_MAGIC_NUM);

          /* we need to understand if it's partial valid FoTA */
          Value32= *(uint32_t *)HeaderAddress;
          HeaderAddress +=4;

          if(Value32==NN_OTA_MAGIC_NUM) {
            uint32_t ActivationSize;
            uint8_t *ModelName;
            OTA_PRINTF("Partial FoTA\r\n");
            HeaderSize = *(uint32_t *)HeaderAddress;
            OTA_PRINTF("Header Size  = %u\r\n", (unsigned int) HeaderSize);
            HeaderAddress+=4;

            OTA_PRINTF("Header Ver   = %u\r\n",*(short unsigned int*)HeaderAddress);
            HeaderAddress+=2;

            OTA_PRINTF("Header Type  = %u\r\n",*(short unsigned int*)HeaderAddress);
            HeaderAddress+=2;

            OTA_PRINTF("Header Sig   = 0x%016llX%016llX\r\n",
                       *(long long unsigned*)(HeaderAddress + 8),
                       *(long long unsigned*)HeaderAddress);
            HeaderAddress+=16;

            OTA_PRINTF("Data Weights = %u\r\n",*(unsigned int*)HeaderAddress);
            HeaderAddress+=4;

            ActivationSize = *(uint32_t *)HeaderAddress;
            OTA_PRINTF("Act Size     = %u\r\n", (unsigned int) ActivationSize);
            HeaderAddress+=4;

            OTA_PRINTF("Reserved     = %0xX%X%X\r\n",
                       *(unsigned int *)(HeaderAddress),
                       *(unsigned int *)(HeaderAddress + 4),
                       *(unsigned int *)(HeaderAddress + 8));

            /* Search if the Activation Size is compatible with one NN */
            DestinationAddress = (uint32_t) aiNetworkRetrieveDataWeightsAddress(ActivationSize,&ModelName);

            if(DestinationAddress==((uint32_t)NULL)) {
              OTA_PRINTF("Error Not A Valid Partial FoTA\r\n");
              OTA_PRINTF("\rThe Update will be rejected\r\n");
              /* This for avoiding to Write the Magic number for BootLoader */
              ValueToWrite=0;
              /* This for Sending the Error To ST BLE sensor Application */
              ReturnValue=-1;
            } else {
              OTA_PRINTF("Valid Partial FoTA for [%s]\r\n", ModelName);
            }
          }else {
            /* Full FOTA update */
            OTA_PRINTF("Full FoTA\r\n");
            HeaderSize = 0;
            DestinationAddress =BootLoaderFeatures->ProgStartAdd;
          }

          if(ReturnValue==1) {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WritingAddress,ValueToWrite)!=HAL_OK) {
              /* Error occurred while writing data in Flash memory.
                 User can add here some code to deal with this error
                 FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
              OTA_ERROR_FUNCTION();
            } else {
              WritingAddress = OTA_MAGIC_NUM_POS+8;
              ValueToWrite=(((uint64_t)DestinationAddress)<<32)| (HeaderSize);
              if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WritingAddress,ValueToWrite)!=HAL_OK) {
                /* Error occurred while writing data in Flash memory.
                   User can add here some code to deal with this error
                   FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
                OTA_ERROR_FUNCTION();
              } else {
									OTA_PRINTF("OTA will be installed at next board reset\r\n");
              }
            }
          }
        } else {
          ReturnValue=-1;
          if(ExpecteduwCRCValue) {
            OTA_PRINTF("Wrong CRC! Computed = %X  Expected = %X ... Try again\r\n",
                       (unsigned) uwCRCValue, (unsigned) ExpecteduwCRCValue);
          }
        }
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
  }
  return ReturnValue;
}

/**
 * @brief Start Function for Updating the Firmware
 * @param uint32_t SizeOfUpdate  size of the firmware image [bytes]
 * @param uint32_t uwCRCValue expected CRC value
 * @retval None
 */
void StartUpdateFWBlueMS(uint32_t SizeOfUpdate, uint32_t uwCRCValue)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  OTA_PRINTF("Start FLASH Erase\r\n");

  SizeOfUpdateBlueFW = SizeOfUpdate;
  ExpecteduwCRCValue = uwCRCValue;
  WritingAddress = OTA_ADDRESS_START;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = GetBank(OTA_MAGIC_NUM_POS);
  EraseInitStruct.Page        = GetPage(OTA_MAGIC_NUM_POS);
  EraseInitStruct.NbPages     = (SizeOfUpdate+16+FLASH_PAGE_SIZE-1)/FLASH_PAGE_SIZE;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

#ifdef STM32L4R9xx
   /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  /* Clear PEMPTY bit set (as the code is executed from Flash which is not empty) */
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0) {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
  }
#endif /* STM32L4R9xx */

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    OTA_ERROR_FUNCTION();
  } else {
    OTA_PRINTF("End FLASH Erase %u Pages of 2KB\r\n", (unsigned int) EraseInitStruct.NbPages);
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
}

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
