/**
  ******************************************************************************
  * @file    mx25r6435f.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MX25R6435F QSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25R6435F_H
#define MX25R6435F_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "mx25r6435f_conf.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup MX25R6435F
  * @{
  */

/** @defgroup MX25R6435F_Exported_Constants MX25R6435F Exported Constants
  * @{
  */

/**
  * @brief  MX25R6435F Size configuration
  */
#define MX25R6435F_BLOCK_64K                            (uint32_t)(64 * 1024)        /* 128 blocks of 64KBytes      */
#define MX25R6435F_BLOCK_32K                            (uint32_t)(32 * 1024)        /* 256 blocks of 32KBytes      */
#define MX25R6435F_SECTOR_4K                            (uint32_t)(4  * 1024)        /* 2048 sectors of 4KBytes     */

#define MX25R6435F_FLASH_SIZE                           (uint32_t)(64*1024*1024/8)   /* 64 Mbits => 8MBytes         */
#define MX25R6435F_PAGE_SIZE                            (uint32_t)256                /* 32768 pages of 256 Bytes    */

/**
  * @brief  MX25R6435F Timing configuration
  */

#define MX25R6435F_CHIP_ERASE_MAX_TIME                  240000U
#define MX25R6435F_BLOCK_64K_ERASE_MAX_TIME             3500U
#define MX25R6435F_BLOCK_32K_ERASE_MAX_TIME             1050U
#define MX25R6435F_SECTOR_4K_ERASE_MAX_TIME             240U
#define MX25R6435F_WRITE_REG_MAX_TIME                   40U

#define MX25R6435F_RESET_MAX_TIME                       100U                 /* when SWreset during chip erase operation */

#define MX25R6435F_AUTOPOLLING_INTERVAL_TIME            0x10U

/**
  * @brief  MX25R6435F Error codes
  */
#define MX25R6435F_OK                                   (0)
#define MX25R6435F_ERROR                                (-1)

/******************************************************************************
  * @brief  MX25R6435F Commands
  ****************************************************************************/
/* Read Operations */
#define MX25R6435F_READ_CMD                             0x03U  /*!< Normal Read                                 */
#define MX25R6435F_FAST_READ_CMD                        0x0BU  /*!< Fast Read Data                              */
#define MX25R6435F_DUAL_OUT_READ_CMD                    0x3BU  /*!< 1I / 20 Read                                */
#define MX25R6435F_DUAL_INOUT_READ_CMD                  0xBBU  /*!< 2 x I/O Read                                */
#define MX25R6435F_QUAD_OUT_READ_CMD                    0x6BU  /*!< 1I / 4O Read                                */
#define MX25R6435F_QUAD_INOUT_READ_CMD                  0xEBU  /*!< 4 x I/O Read                                */

/* Program Operations */
#define MX25R6435F_PAGE_PROG_CMD                        0x02U  /*!< Page Program                                */
#define MX25R6435F_QUAD_PAGE_PROG_CMD                   0x38U  /*!< Quad Page Program                           */

/* Erase Operations */
#define MX25R6435F_SECTOR_ERASE_CMD                     0x20U  /*!< Sector Erase                                */
#define MX25R6435F_SUBBLOCK_ERASE_CMD                   0x52U  /*!< Block Erase 32KB                            */
#define MX25R6435F_BLOCK_ERASE_CMD                      0xD8U  /*!< Block Erase 64KB                            */
#define MX25R6435F_CHIP_ERASE_CMD                       0x60U  /*!< Chip Erase                                  */
#define MX25R6435F_CHIP_ERASE_CMD_2                     0xC7U  /*!< Chip Erase                                  */

#define MX25R6435F_PROG_ERASE_RESUME_CMD                0x7AU  /*!< Resume Program/Erase                        */
#define MX25R6435F_PROG_ERASE_RESUME_CMD_2              0x30U  /*!< Resume Program/Erase                        */
#define MX25R6435F_PROG_ERASE_SUSPEND_CMD               0x75U  /*!< Suspend Program/Erase                       */
#define MX25R6435F_PROG_ERASE_SUSPEND_CMD_2             0xB0U  /*!< Suspend Program/Erase                       */

/* Identification Operations */
#define MX25R6435F_READ_ID_CMD                          0x9FU  /*!< Read Identification                         */
#define MX25R6435F_READ_ELECTRONIC_ID_CMD               0xABU  /*!< Read Electronic ID                          */
#define MX25R6435F_READ_ELEC_MANUFACTURER_DEVICE_ID_CMD 0x90U  /*!< Read Electronic Manufacturuer & Device ID   */
#define MX25R6435F_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AU  /*!< Read SFDP                                   */

/* Write Operations */
#define MX25R6435F_WRITE_ENABLE_CMD                     0x06U  /*!< Write Enable                                */
#define MX25R6435F_WRITE_DISABLE_CMD                    0x04U  /*!< Write Disable                               */

/* Register Operations */
#define MX25R6435F_READ_STATUS_REG_CMD                  0x05U  /*!< Read Status Register                        */
#define MX25R6435F_READ_CFG_REG_CMD                     0x15U  /*!< Read Configuration Register                 */
#define MX25R6435F_WRITE_STATUS_CFG_REG_CMD             0x01U  /*!< Write Status Register                       */

#define MX25R6435F_READ_SEC_REG_CMD                     0x2BU  /*!< Read Security Register                      */
#define MX25R6435F_WRITE_SEC_REG_CMD                    0x2FU  /*!< Write Security Register                     */

/* Power Down Operations */
#define MX25R6435F_DEEP_POWER_DOWN_CMD                  0xB9U  /*!< Deep Power Down                             */

/* Burst Operations */
#define MX25R6435F_SET_BURST_LENGTH_CMD                 0xC0U  /*!< Set Burst Length                            */

/* One-Time Programmable Operations */
#define MX25R6435F_ENTER_SECURED_OTP_CMD                0xB1U  /*!< Enter Secured OTP                           */
#define MX25R6435F_EXIT_SECURED_OTP_CMD                 0xC1U  /*!< Exit Secured OTP                            */

/* No Operation */
#define MX25R6435F_NO_OPERATION_CMD                     0x00U  /*!< No Operation                                */

/* Reset Operations */
#define MX25R6435F_RESET_ENABLE_CMD                     0x66U  /*!< Reset Enable                                */
#define MX25R6435F_RESET_MEMORY_CMD                     0x99U  /*!< Reset Memory                                */
#define MX25R6435F_RELEASE_READ_ENHANCED_CMD            0xFFU  /*!< Release Read Enhanced                       */

/******************************************************************************
  * @brief  MX25R6435F Registers
  ****************************************************************************/
/* Status Register */
#define MX25R6435F_SR_WIP                               0x01U  /*!< Write in progress                           */
#define MX25R6435F_SR_WEL                               0x02U  /*!< Write enable latch                          */
#define MX25R6435F_SR_BP                                0x3CU  /*!< Block protect                               */
#define MX25R6435F_SR_QE                                0x40U  /*!< Quad enable                                 */
#define MX25R6435F_SR_SRWD                              0x80U  /*!< Status register write disable               */

/* Configuration Register 1 */
#define MX25R6435F_CR1_TB                               0x08U  /*!< Top / bottom selected                       */

/* Configuration Register 2 */
#define MX25R6435F_CR2_LH_SWITCH                        0x02U  /*!< Low power / high performance switch         */

/* Security Register */
#define MX25R6435F_SECR_SOI                             0x01U  /*!< Secured OTP indicator                       */
#define MX25R6435F_SECR_LDSO                            0x02U  /*!< Lock-down secured OTP                       */
#define MX25R6435F_SECR_PSB                             0x04U  /*!< Program suspend bit                         */
#define MX25R6435F_SECR_ESB                             0x08U  /*!< Erase suspend bit                           */
#define MX25R6435F_SECR_P_FAIL                          0x20U  /*!< Program fail flag                           */
#define MX25R6435F_SECR_E_FAIL                          0x40U  /*!< Erase fail flag                             */

/**
  * @}
  */ 

/** @defgroup MX25R6435F_Exported_Types MX25R6435F Exported Types
  * @{
  */
typedef struct {
  uint32_t FlashSize;                                          /*!< Size of the flash                           */
  uint32_t EraseBlockSize;                                     /*!< Size of block for the erase operation       */
  uint32_t EraseBlocksNumber;                                  /*!< Number of block for the erase operation     */
  uint32_t EraseSubBlockSize;                                  /*!< Size of sub-block for the erase operation   */
  uint32_t EraseSubBlocksNumber;                               /*!< Number of sub-block for the erase operation */
  uint32_t EraseSectorSize;                                    /*!< Size of sectors for the erase operation     */
  uint32_t EraseSectorsNumber;                                 /*!< Number of sectors for the erase operation   */
  uint32_t ProgPageSize;                                       /*!< Size of pages for the program operation     */
  uint32_t ProgPagesNumber;                                    /*!< Number of pages for the program operation   */
} MX25R6435F_Info_t;

typedef enum {
  MX25R6435F_SPI_MODE = 0,                                     /*!< 1-1-1 commands, Poweron H/W default setting */
  MX25R6435F_DUAL_OUT_MODE,                                    /*!< 1-1-2 commands                              */
  MX25R6435F_DUAL_IO_MODE,                                     /*!< 1-2-2 commands                              */
  MX25R6435F_QUAD_OUT_MODE,                                    /*!< 1-1-4 commands                              */
  MX25R6435F_QUAD_IO_MODE                                      /*!< 1-4-4 commands                              */
} MX25R6435F_Interface_t;

typedef enum {
  MX25R6435F_ERASE_4K = 0,                                    /*!< 4K size Sector erase                          */
  MX25R6435F_ERASE_32K,                                       /*!< 32K size Block erase                          */
  MX25R6435F_ERASE_64K,                                       /*!< 64K size Block erase                          */
  MX25R6435F_ERASE_CHIP                                       /*!< Whole chip erase                              */
} MX25R6435F_Erase_t;

/**
  * @}
  */ 

/** @defgroup MX25R6435F_Exported_Functions MX25R6435F Exported Functions
  * @{
  */ 
/* Function by commands combined */
int32_t MX25R6435F_GetFlashInfo(MX25R6435F_Info_t *pInfo);
int32_t MX25R6435F_AutoPollingMemReady(QSPI_HandleTypeDef *Ctx);

/* Read/Write Array Commands **************************************************/
int32_t MX25R6435F_Read(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX25R6435F_PageProgram(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t MX25R6435F_BlockErase(QSPI_HandleTypeDef *Ctx, uint32_t BlockAddress, MX25R6435F_Erase_t BlockSize);
int32_t MX25R6435F_ChipErase(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_EnableMemoryMappedMode(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode);
int32_t MX25R6435F_Suspend(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_Resume(QSPI_HandleTypeDef *Ctx);

/* Register/Setting Commands **************************************************/
int32_t MX25R6435F_WriteEnable(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_WriteDisable(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value);
int32_t MX25R6435F_WriteStatusRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value);
int32_t MX25R6435F_WriteCfgRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value);
int32_t MX25R6435F_ReadCfgRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value);
int32_t MX25R6435F_WriteCfg2Register(QSPI_HandleTypeDef *Ctx, uint8_t Value);
int32_t MX25R6435F_ReadCfg2Register(QSPI_HandleTypeDef *Ctx, uint8_t *Value);
int32_t MX25R6435F_WriteSecurityRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value);
int32_t MX25R6435F_ReadSecurityRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value);

/* ID/Security Commands *******************************************************/
int32_t MX25R6435F_ReadID(QSPI_HandleTypeDef *Ctx, uint8_t *ID);

/* Reset Commands *************************************************************/
int32_t MX25R6435F_ResetEnable(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_ResetMemory(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_NoOperation(QSPI_HandleTypeDef *Ctx);
int32_t MX25R6435F_EnterPowerDown(QSPI_HandleTypeDef *Ctx);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* MX25R6435F_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
