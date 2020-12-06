/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version V4.0.0
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.
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
#include "usbd_storage_if.h"
#include "stm32l475e_iot01_qspi.h"
#include "stm32l475e_iot01_qspi_ex.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */


/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1

// #define USE_EMBEDDED_SRAM /* Can be used for debug purposes */

#ifdef USE_EMBEDDED_SRAM
#define STORAGE_BLK_NBR                  0xC
// #define STORAGE_BLK_SIZ                  0x1000 /* 4096 */
// #define STORAGE_BLK_SIZ                  0x800 /* 2048 */
#define STORAGE_BLK_SIZ                  0x200  /* 512 */
static uint8_t mscRamMemory[STORAGE_BLK_NBR * STORAGE_BLK_SIZ];
#else
#define STORAGE_BLK_NBR                  (MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_4K -  1) /* 2048 - 1 */
#define STORAGE_BLK_SIZ                  MX25R6435F_SECTOR_4K  /* 4096 */
#endif /* USE_EMBEDDED_SRAM */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_FS[] = {/* 36 */
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
};

/**
  * @}
  */


/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initializes over USB FS IP
 * @param  lun: Logical unit number
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
int8_t STORAGE_Init_FS(uint8_t lun)
{
  uint8_t status;
  BSP_QSPI_Init_t QSPI_InitStruct;

  /* QSPI device configuration */
  // QSPI_InitStruct.InterfaceMode = BSP_QSPI_SPI_MODE;
  QSPI_InitStruct.InterfaceMode = BSP_QSPI_QUAD_IO_MODE;
  status = BSP_QSPI_Init(0, &QSPI_InitStruct);

  if (status != BSP_ERROR_NONE)
  {
    USBD_UsrLog("[USB STORAGE] QSPI Init: FAILED, QSPI Test Aborted\r\n");
    status = USBD_FAIL;
  }
  else
  {
    USBD_UsrLog("[USB STORAGE] QSPI Init: OK\r\n");
    status = USBD_OK;
  }

  USBD_UsrLog("[USB STORAGE] Init ---------------------------------------\r\n");

  return status;
}

/**
 * @brief  Returns the medium capacity.
 * @param  lun: Logical unit number
 * @param  block_num: Number of total block number
 * @param  block_size: Block size
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  *block_num  = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
}

/**
 * @brief  Checks whether the medium is ready.
 * @param  lun: Logical unit number
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
int8_t STORAGE_IsReady_FS(uint8_t lun)
{
  int8_t ret;
  if (BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE) {
    ret = USBD_BUSY;
    USBD_UsrLog("[USB STORAGE] Busy\r\n");
  } else {
    ret = USBD_OK;
    USBD_UsrLog("[USB STORAGE] IsReady\r\n");
  }
  return ret;
}

/**
 * @brief  Checks whether the medium is write protected.
 * @param  lun: Logical unit number
 * @retval Status (0: write enabled / -1: otherwise)
 */
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
  return (USBD_OK); /* Write enabled */
  //return -1; /* Read-only */
}

/**
 * @brief  Reads data from the medium.
 * @param  lun: Logical unit number
 * @param  buf: pointer to data
 * @param  blk_addr: Logical block address
 * @param  blk_len: Blocks number
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  uint32_t buffer_size = (STORAGE_BLK_SIZ * blk_len);
  uint32_t read_addr = (STORAGE_BLK_SIZ * blk_addr);
  USBD_UsrLog("[USB STORAGE] Read sector %lu, blk_len = %u\r\n", blk_addr, blk_len);
  USBD_UsrLog("[USB STORAGE] Read %lu Bytes @ %#lx\r\n", buffer_size, read_addr);

#ifdef USE_EMBEDDED_SRAM
  memcpy(buf, &mscRamMemory[read_addr], buffer_size);
  return USBD_OK;
#else
  if (blk_len == 0) return USBD_OK;

  uint32_t timeout = 100000;

  if (BSP_QSPI_Read(0, buf, read_addr, buffer_size) != BSP_ERROR_NONE)
  {
    return USBD_FAIL;
  }
  while(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
  {
    if (timeout-- == 0)
    {
      USBD_UsrLog("[USB STORAGE] Read FAIL\r\n");
      return USBD_FAIL;
    }
  }
  USBD_UsrLog("[USB STORAGE] Read Ok\r\n");

  return (USBD_OK);
#endif /* USE_EMBEDDED_SRAM */
}

/**
 * @brief  Writes data into the medium.
 * @param  lun: Logical unit number
 * @param  buf: pointer to data
 * @param  blk_addr: Logical block address
 * @param  blk_len: Blocks number
 * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  uint32_t buffer_size = (STORAGE_BLK_SIZ * blk_len);
  uint32_t write_addr = (STORAGE_BLK_SIZ * blk_addr);
  USBD_UsrLog("[USB STORAGE] Erase sector %lu, blk_len = %u\r\n", blk_addr, blk_len);
  USBD_UsrLog("[USB STORAGE] Write %lu Bytes @ %#lx\r\n", buffer_size, write_addr);

#ifdef USE_EMBEDDED_SRAM
  memcpy(&mscRamMemory[write_addr], buf, buffer_size);
  return USBD_OK;
#else
  uint32_t timeout = 100000;

  if (blk_len != 1) {
    USBD_UsrLog("[USB STORAGE] blk_len !=1 not supported\r\n");
    while(1);
  }

  if (BSP_QSPI_Erase_Block(0, write_addr, BSP_QSPI_ERASE_4K) != BSP_ERROR_NONE)
  {
    USBD_UsrLog("[USB STORAGE] Erase Sector FAIL\r\n");
    return USBD_FAIL;
  }
  BSP_QSPIEx_WaitForEndOfOperation(0, 300);

  while(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
  {
    if (timeout-- == 0)
    {
      USBD_UsrLog("[USB STORAGE] Erase wait FAIL\r\n");
      return USBD_FAIL;
    }
  }
  USBD_UsrLog("[USB STORAGE] Erase Ok\r\n");

  timeout = 100000;
  if (BSP_QSPI_Write(0, buf, write_addr, buffer_size) != BSP_ERROR_NONE)
  {
    USBD_UsrLog("[USB STORAGE] Write FAIL\r\n");
    return USBD_FAIL;
  }

  BSP_QSPIEx_WaitForEndOfOperation(0, 300);

  while(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
  {
    if (timeout-- == 0)
    {
      USBD_UsrLog("[USB STORAGE] Write wait FAIL\r\n");
      return USBD_FAIL;
    }
  }
  USBD_UsrLog("[USB STORAGE] Write Ok\r\n");

  return (USBD_OK);
#endif /* USE_EMBEDDED_SRAM */

}

/**
 * @brief  Returns the Max Supported LUNs.
 * @param  None
 * @retval Lun(s) number
 */
int8_t STORAGE_GetMaxLun_FS(void)
{
  return (STORAGE_LUN_NBR - 1);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
