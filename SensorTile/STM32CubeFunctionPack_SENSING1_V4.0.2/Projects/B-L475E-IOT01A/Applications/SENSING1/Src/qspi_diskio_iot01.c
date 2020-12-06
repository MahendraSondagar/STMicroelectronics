/**
 ******************************************************************************
  * @file    qspi_diskio_iot01.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   QSPI Disk I/O driver for IoT node
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
#include <string.h>
#include "ff_gen_drv.h"
#include "stm32l475e_iot01_qspi.h"
#include "stm32l475e_iot01_qspi_ex.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS QSPI_initialize (BYTE pdrv);
DSTATUS QSPI_status (BYTE pdrv);
DRESULT QSPI_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT QSPI_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT QSPI_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  QSPI_Driver =
{
  QSPI_initialize,
  QSPI_status,
  QSPI_read,
#if  _USE_WRITE
  QSPI_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  QSPI_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS QSPI_initialize (
	BYTE pdrv           /* Physical drive number to identify the drive */
)
{
  uint32_t status;
  BSP_QSPI_Init_t QSPI_InitStruct;

  Stat = STA_NOINIT;

  /* QSPI device configuration */
  // QSPI_InitStruct.InterfaceMode = BSP_QSPI_SPI_MODE;
  QSPI_InitStruct.InterfaceMode = BSP_QSPI_QUAD_IO_MODE;
  status = BSP_QSPI_Init(0, &QSPI_InitStruct);

  if (status != BSP_ERROR_NONE)
  {
    Stat = RES_ERROR;
  }
  else
  {
    Stat = RES_OK;
  }

  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS QSPI_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  if (BSP_QSPI_GetStatus(0) == BSP_ERROR_NONE) {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT QSPI_read (
	BYTE pdrv,      /* Physical drive number to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  uint32_t BufferSize = (MX25R6435F_SECTOR_4K * count) / 4; // Read size?
  uint32_t read_addr = (MX25R6435F_SECTOR_4K * sector);

  if (BSP_QSPI_Read(0, buff, read_addr, BufferSize) != BSP_ERROR_NONE)
  {
    return RES_ERROR;
  }

  return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT QSPI_write (
	BYTE pdrv,          /* Physical drive number to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  uint32_t BufferSize = (MX25R6435F_SECTOR_4K * count);
  uint32_t write_addr = (MX25R6435F_SECTOR_4K * sector);

  if (BSP_QSPI_Erase_Block(0, write_addr, BSP_QSPI_ERASE_4K) != BSP_ERROR_NONE)
  {
    return RES_ERROR;
  }
  BSP_QSPIEx_WaitForEndOfOperation(0, 300);

  // HAL_Delay(50);

  if (BSP_QSPI_Write(0, (uint8_t *) buff, write_addr, BufferSize) != BSP_ERROR_NONE)
  {
    return RES_ERROR;
  }
  BSP_QSPIEx_WaitForEndOfOperation(0, 300);

  return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT QSPI_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  DRESULT res = RES_ERROR;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC:
    if (BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE) {
      res = RES_ERROR;
    } else {
      res = RES_OK;
    }
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT:
    *(DWORD*)buff = MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_4K; /* 2048 Sectors of 4kBytes */
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE:
    *(DWORD*)buff = MX25R6435F_SECTOR_4K; /* 0x1000 = 4kBytes */
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE:
    *(DWORD*)buff = MX25R6435F_BLOCK_64K / MX25R6435F_SECTOR_4K; /* 16 sector blocks */
    res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
