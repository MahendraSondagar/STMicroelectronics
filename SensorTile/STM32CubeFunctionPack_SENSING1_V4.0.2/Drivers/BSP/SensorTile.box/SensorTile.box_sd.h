/**
******************************************************************************
* @file    SensorTile.box_sd.h (based on stm32l4r9i_eval_sd.h)
* @author  SRA - Central Labs
* @version V1.2.0
* @date    01-Sept-2019
* @brief   This file contains the common defines and functions prototypes for 
*          the SensorTile.box_sd.c driver.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORTILEBOX_SD_H
#define __SENSORTILEBOX_SD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/** @defgroup SENSORTILE_SD_Exported_Constants SENSORTILE_SD Exported Constants
  * @{
  */ 

/** 
  * @brief  SD transfer state definition  
  */     
#define SD_TRANSFER_OK                ((uint8_t)0x00)
#define SD_TRANSFER_BUSY              ((uint8_t)0x01)
#define SD_TRANSFER_ERROR             ((uint8_t)0x02)

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

/**
  * @brief  GPIO for SD Detect PIn
  */  
#define SD_DETECT_GPIO_Pin  GPIO_PIN_12
#define SD_DETECT_GPIO_Port GPIOB

/**
  * @brief  GPIO for SD Select PIN
  */  
#define SD_SEL_Pin  GPIO_PIN_5
#define SD_SEL_Port GPIOE

/**
  * @brief  SD status structure definition  
  */     
#define MSD_OK                        ((uint8_t)0x00)
#define MSD_ERROR                     ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)
#define SD_DATATIMEOUT                  ((uint32_t)100000000)

/** 
  * @brief SD Card information 
  */
#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

/**
  * @}
  */
  

/** @defgroup SENSORTILE_SD_Exported_Functions SENSORTILE_SD Exported Functions
  * @{
  */   
/* Exported functions --------------------------------------------------------*/   
uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_IsDetected(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t BSP_SD_GetCardState(void);
void    BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo);

uint8_t BSP_SD_ITConfig(void);
void    BSP_SD_DetectIT(void);
void    BSP_SD_DetectCallback(void);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);


/* These functions can be modified in case the current settings (eg. interrupt priority)
   need to be changed for specific application needs */
void    BSP_SD_AbortCallback(void);
void    BSP_SD_WriteCpltCallback(void);
void    BSP_SD_ReadCpltCallback(void);

/** @defgroup SENSORTILE_SD_Exported_Variables SENSORTILE_SD Exported Variables
  * @{
  */   
/* Exported Variables --------------------------------------------------------*/
extern SD_HandleTypeDef hsd1;
extern __IO uint8_t SD_Status;

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __SENSORTILEBOX_SD_H */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
