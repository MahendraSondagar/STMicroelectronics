/**
  ******************************************************************************
  * @file    main.c
  * @author  Central LAB
  * @version $Version$
  * @date    $Date$
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
/* Private typedef -----------------------------------------------------------*/

typedef struct
{
  uint32_t Version;
  uint32_t MagicNum;
  uint32_t FwUpdateStartAdd;
  uint32_t FwDoneAdd;
  uint32_t FwMaxSize;
  uint32_t ProgStartAdd;
} BootLoaderFeatures_t;


/* Private define ------------------------------------------------------------*/
#define BL_VERSION_MAJOR 2
#define BL_VERSION_MINOR 0
#define BL_VERSION_PATCH 0

#define OTA_MAGIC_NUM ((uint32_t)0xDEADBEEF)

/* Running program Position */
#define PROG_ADDRESS_START 0x08004000

/* remove only the Magic Number (2bytes) */
#define OTA_NUM_PAGES  1

#ifdef STM32L4R9xx

  /* Max Program Size */
  #define MAX_PROG_SIZE (0xFFFFF-0x3FFF)

  /* Board  FW OTA Magic Number Position */
  #define OTA_ADDRESS_START  0x08100000

  /* Board  FW OTA DONE Magic Number Position */
  #define OTA_MAGIC_DONE_NUM_POS  0x08100008

  /* Running program */
  /* 1004 Kbytes */
  #define PROG_NUM_PAGES 251

  #ifdef STM32_SENSORTILEBOX
    #define MCR_HEART_BIT() \
    { \
      BSP_LED_On(LED1);\
      BSP_LED_On(LED2);\
      HAL_Delay(200);\
      BSP_LED_Off(LED1);\
      BSP_LED_Off(LED2);\
      HAL_Delay(400);\
      BSP_LED_On(LED1);\
      BSP_LED_On(LED2);\
      HAL_Delay(200);\
      BSP_LED_Off(LED1);\
      BSP_LED_Off(LED2);\
      HAL_Delay(1000);\
    }
  #endif /* STM32_SENSORTILEBOX */
#else /* STM32L4R9xx */

  /* Max Program Size */
  #define MAX_PROG_SIZE (0x7FFFF-0x3FFF)

  /* Board  FW OTA Magic Number Position */
  #define OTA_ADDRESS_START  0x08080000

  /* Board  FW OTA DONE Magic Number Position */
  #define OTA_MAGIC_DONE_NUM_POS  0x08080008

  /* Running program */
  /* 496 Kbytes */
  #define PROG_NUM_PAGES 248

#endif /* STM32L4R9xx */

/* Private variables ---------------------------------------------------------*/
#pragma location=".version"
__root const BootLoaderFeatures_t BootLoaderFeatures={((BL_VERSION_MAJOR<<16) | (BL_VERSION_MINOR<<8) | BL_VERSION_PATCH),
                                                      OTA_MAGIC_NUM,
                                                      OTA_ADDRESS_START,
                                                      OTA_MAGIC_DONE_NUM_POS,
                                                      MAX_PROG_SIZE,
                                                      PROG_ADDRESS_START};

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static uint32_t GetPage(uint32_t Addr);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t SourceAddress = OTA_ADDRESS_START;
  uint32_t data32 = *(uint32_t*) SourceAddress;

  /* Check if there is Full/Partial Firmware Update */
  if(data32==OTA_MAGIC_NUM){
    /* Make the Firmware Update*/

    /* Update Size */
    uint32_t SizeOfUpdate;

    /* Firmware Update Header Size */
    uint32_t FwHeaderSize;

    /* First Destination Addres to change */
    uint32_t FwDestAddress;

    /* This is the last Address to change */
    uint32_t LastFwDestAddress;

    uint32_t CurrentPageAddress;

    /* TmpBuffer for storing one Flash Page :
     * 0x1000 == 4096 for L4R9
     * 0x800  == 2048 for L47x */
    uint64_t FlashPageBuffer[FLASH_PAGE_SIZE>>3];
    uint8_t *FlashPageBuffer8Bit = (uint8_t *)FlashPageBuffer;

    HAL_Init();

    /* Configure the System clock */
    SystemClock_Config();

    /* Update Size */
    SizeOfUpdate = *(uint32_t*) (SourceAddress+4);

    /* Dimension of Update header */
    FwHeaderSize = *(uint32_t*) (SourceAddress+8);

    /* Address of the Flash region that we need to update */
    FwDestAddress = *(uint32_t*) (SourceAddress+12);

    if(SizeOfUpdate==0) {
      /* If there is not the dimension of the Update... we set the Full program Size */
      SizeOfUpdate = MAX_PROG_SIZE;
    } else {
      /* This is the Real Size the Partial Firmware Update */
      SizeOfUpdate -=FwHeaderSize;
    }

#ifdef STM32_SENSORTILEBOX
    /* Init Led1 */
    BSP_LED_Init(LED1);

    /* Init Led2 */
    BSP_LED_Init(LED2);

    /* Make a heart bit:
     * Single for Partial Firmware Update
     * Double for Full Firmware Update
    */
    MCR_HEART_BIT();
#endif /* STM32_SENSORTILEBOX */
    if(FwDestAddress==PROG_ADDRESS_START) {
#ifdef STM32_SENSORTILEBOX
      MCR_HEART_BIT();
#endif /* STM32_SENSORTILEBOX */
      /* if the Size of Update is not present */
      if(SizeOfUpdate==0x0) {
        SizeOfUpdate  = MAX_PROG_SIZE;
      }
    }

    /* Last Destination Address that we need to change */
    LastFwDestAddress = FwDestAddress + SizeOfUpdate;

    /* Source Address */
    SourceAddress +=16+FwHeaderSize;

    /* Loop Cycle for writing the Firmare Update  */
    while(FwDestAddress<LastFwDestAddress) {
      uint32_t Counter;
      FLASH_EraseInitTypeDef EraseInitStruct;
      uint32_t SectorError = 0;

      /* These are the Address of the Current Flash page */
      CurrentPageAddress = FwDestAddress  & (~(FLASH_PAGE_SIZE-1));
#ifdef STM32_SENSORTILEBOX
      BSP_LED_On(LED1);
      BSP_LED_Off(LED2);
#endif /* STM32_SENSORTILEBOX */

      /******************* 1) Copy Data in memory *****************************/
      /* Copy the Content of the Flash page in the Tmp Buffer */
      for(Counter=0; Counter<(FLASH_PAGE_SIZE);Counter++) {
        FlashPageBuffer8Bit[Counter] =  *(((uint8_t*) CurrentPageAddress) + Counter);
      }

      /*********************** 2) Erase Flash *********************************/
      EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.Banks       = FLASH_BANK_1;
      EraseInitStruct.Page        = GetPage(FwDestAddress);

      EraseInitStruct.NbPages = 1;

      /* Unlock the Flash */
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
        /* Error occurred during erase section */
        uint32_t errorcode = HAL_FLASH_GetError();
        while(1);
      }

      /*********************** 3) Update Data in memory ***********************/

      Counter = FwDestAddress & (FLASH_PAGE_SIZE-1);
      for(;((Counter<(FLASH_PAGE_SIZE)) & (FwDestAddress<LastFwDestAddress)); Counter++) {
        FlashPageBuffer8Bit[Counter] = *((uint8_t *) (SourceAddress));
        FwDestAddress++;
        SourceAddress++;
      }
      
      /*********************** 4) Write Data in Flash**************************/

      for(Counter=0; Counter<(FLASH_PAGE_SIZE);Counter+=8) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, CurrentPageAddress+Counter,FlashPageBuffer[Counter>>3]) != HAL_OK){
          /* Error occurred during writing */
          uint32_t errorcode = HAL_FLASH_GetError();
          while(1);
        }
      }

      /* Lock the Flash */
      HAL_FLASH_Lock();
#ifdef STM32_SENSORTILEBOX
      BSP_LED_Off(LED1);
      BSP_LED_On(LED2);
#endif /* STM32_SENSORTILEBOX */
    }

    /******************* Delete/Add Magic Number ***************************/
    {
      FLASH_EraseInitTypeDef EraseInitStruct;
      uint32_t SectorError = 0;

      /* Unlock the Flash */
      HAL_FLASH_Unlock();

      /* Reset the Second half Flash */
      EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.Banks       = FLASH_BANK_2;
      EraseInitStruct.Page        = GetPage(OTA_ADDRESS_START);
      EraseInitStruct.NbPages     = 1;
#ifdef STM32L4R9xx
      /* Clear OPTVERR bit set on virgin samples */
      __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
      /* Clear PEMPTY bit */
      if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0) {
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
      }
#endif /* STM32L4R9xx */

      /* Delete the Magic Number Used for FOTA */
      if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        /* Error occurred during erase section */
        uint32_t errorcode = HAL_FLASH_GetError();
        while(1);
      }

      /* Lock the Flash */
      HAL_FLASH_Lock();
    }
#ifdef STM32_SENSORTILEBOX
    BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);
#endif /* STM32_SENSORTILEBOX */

    /* System Reboot */
    HAL_NVIC_SystemReset();
  } else {
    /* Jump To Normal boot */
    typedef  void (*pFunction)(void);

    pFunction JumpToApplication;
    uint32_t JumpAddress;

    /* reset all interrupts to default */
    // __disable_irq();

    /* Jump to system memory */
    JumpAddress = *(__IO uint32_t*) (PROG_ADDRESS_START + 4);
    JumpToApplication = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) PROG_ADDRESS_START);
    JumpToApplication();
  }
}

#ifdef STM32_SENSORTILEBOX
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB
                              |RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLP;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }
}
#else /* STM32_SENSORTILEBOX */
/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }
}
#endif /* STM32_SENSORTILEBOX */

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  } else {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
