/**
 ******************************************************************************
 * @file    mx25r6435f.c
 * @modify  MCD Application Team
 * @brief   This file provides the MX25R6435F QSPI driver.
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

/* Includes ------------------------------------------------------------------*/
#include "mx25r6435f.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup MX25R6435F MX25R6435F
  * @{
  */

/** @defgroup MX25R6435F_Exported_Functions MX25R6435F Exported Functions
  * @{
  */

/**
  * @brief  Get Flash information
  * @param  pInfo pointer to information structure
  * @retval error status
  */
int32_t MX25R6435F_GetFlashInfo(MX25R6435F_Info_t *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize              = MX25R6435F_FLASH_SIZE;
  pInfo->EraseBlockSize         = MX25R6435F_BLOCK_64K;
  pInfo->EraseBlocksNumber      = (MX25R6435F_FLASH_SIZE/MX25R6435F_BLOCK_64K);
  pInfo->EraseSubBlockSize      = MX25R6435F_BLOCK_32K;
  pInfo->EraseSubBlocksNumber   = (MX25R6435F_FLASH_SIZE/MX25R6435F_BLOCK_32K);
  pInfo->EraseSectorSize        = MX25R6435F_SECTOR_4K;
  pInfo->EraseSectorsNumber     = (MX25R6435F_FLASH_SIZE/MX25R6435F_SECTOR_4K);
  pInfo->ProgPageSize           = MX25R6435F_PAGE_SIZE;
  pInfo->ProgPagesNumber        = (MX25R6435F_FLASH_SIZE/MX25R6435F_PAGE_SIZE);

  return MX25R6435F_OK;
};

/**
  * @brief  Polling WIP(Write In Progress) bit become to 0
  *         SPI/OPI;
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  Rate Transfer rate
  * @retval error status
  */
int32_t MX25R6435F_AutoPollingMemReady(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Configure automatic polling mode to wait for memory ready */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_STATUS_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0U;
  s_config.Mask            = MX25R6435F_SR_WIP;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.Interval        = MX25R6435F_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  s_config.StatusBytesSize = 1;

  if (HAL_QSPI_AutoPolling(Ctx, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/* Read/Write Array Commands ****************************************************/
/**
  * @brief  Reads an amount of data from the QSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MX25R6435F_Read(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressSize        = QSPI_ADDRESS_24_BITS;
  s_command.Address            = ReadAddr;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.NbData             = Size;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
  
  switch(Mode)
  {
  case MX25R6435F_SPI_MODE :
    s_command.Instruction = MX25R6435F_FAST_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_1_LINE;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_DUAL_OUT_MODE :
    s_command.Instruction = MX25R6435F_DUAL_OUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_2_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_DUAL_IO_MODE :
    s_command.Instruction = MX25R6435F_DUAL_INOUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_2_LINES;
    s_command.DataMode    = QSPI_DATA_2_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ_DUAL;
    break;
    
  case MX25R6435F_QUAD_OUT_MODE :
    s_command.Instruction = MX25R6435F_QUAD_OUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_4_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_QUAD_IO_MODE :
    s_command.Instruction = MX25R6435F_QUAD_INOUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.DataMode    = QSPI_DATA_4_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ_QUAD;
    break;
    
  default :
    return MX25R6435F_ERROR;	  
  }
  
  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  *         SPI/QUAD_INOUT/; 1-1-1/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ MX25R6435F_PAGE_SIZE
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MX25R6435F_PageProgram(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the program command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressSize        = QSPI_ADDRESS_24_BITS;
  s_command.Address            = WriteAddr;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.NbData             = Size;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
  
  switch(Mode)
  {
  case MX25R6435F_SPI_MODE :
    s_command.Instruction = MX25R6435F_PAGE_PROG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_1_LINE;
    break;
    
  case MX25R6435F_QUAD_IO_MODE :
    s_command.Instruction = MX25R6435F_QUAD_PAGE_PROG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.DataMode    = QSPI_DATA_4_LINES;
    break;
    
  default :
    return MX25R6435F_ERROR;	  
  }
  
  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         MX25R6435F support 4K, 32K and 64K size block erase commands.
  * @param  Ctx Component object pointer
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Block size to erase
  * @retval QSPI memory status
  */
int32_t MX25R6435F_BlockErase(QSPI_HandleTypeDef *Ctx, uint32_t BlockAddress, MX25R6435F_Erase_t BlockSize)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the erase command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressMode        = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize        = QSPI_ADDRESS_24_BITS;
  s_command.Address            = BlockAddress;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
  
  switch(BlockSize)
  {
  case MX25R6435F_ERASE_4K :
    s_command.Instruction = MX25R6435F_SECTOR_ERASE_CMD;
    break;
    
  case MX25R6435F_ERASE_32K :
    s_command.Instruction = MX25R6435F_SUBBLOCK_ERASE_CMD;
    break;
    
  case MX25R6435F_ERASE_64K :
    s_command.Instruction = MX25R6435F_BLOCK_ERASE_CMD;
    break;
    
  default :
    return MX25R6435F_ERROR;	  
  }
  
  /* Send the command */
  if(HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Whole chip erase.
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval error status
  */
int32_t MX25R6435F_ChipErase(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the erase command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_CHIP_ERASE_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
  
  /* Send the command */
  if(HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Enable memory mapped mode for the QSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25R6435F_EnableMemoryMappedMode(QSPI_HandleTypeDef *Ctx, MX25R6435F_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* Initialize the read command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.AddressSize        = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
  
  switch(Mode)
  {
  case MX25R6435F_SPI_MODE :
    s_command.Instruction = MX25R6435F_FAST_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_1_LINE;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_DUAL_OUT_MODE :
    s_command.Instruction = MX25R6435F_DUAL_OUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_2_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_DUAL_IO_MODE :
    s_command.Instruction = MX25R6435F_DUAL_INOUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_2_LINES;
    s_command.DataMode    = QSPI_DATA_2_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ_DUAL;
    break;
    
  case MX25R6435F_QUAD_OUT_MODE :
    s_command.Instruction = MX25R6435F_QUAD_OUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.DataMode    = QSPI_DATA_4_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ;
    break;
    
  case MX25R6435F_QUAD_IO_MODE :
    s_command.Instruction = MX25R6435F_QUAD_INOUT_READ_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.DataMode    = QSPI_DATA_4_LINES;
    s_command.DummyCycles = DUMMY_CYCLES_READ_QUAD;
    break;
    
  default :
    return MX25R6435F_ERROR;	  
  }
  
  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_QSPI_MemoryMapped(Ctx, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Flash suspend program or erase command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_Suspend(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the suspend command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_PROG_ERASE_SUSPEND_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Flash resume program or erase command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_Resume(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the resume command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_PROG_ERASE_RESUME_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/* Register/Setting Commands **************************************************/
/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_WriteEnable(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Initialize the write enable command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_ENABLE_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  s_command.Instruction = MX25R6435F_READ_STATUS_REG_CMD;
  s_command.DataMode    = QSPI_DATA_1_LINE;

  s_config.Match           = MX25R6435F_SR_WEL;
  s_config.Mask            = MX25R6435F_SR_WEL;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.Interval        = MX25R6435F_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  s_config.StatusBytesSize = 1;

  if (HAL_QSPI_AutoPolling(Ctx, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  This function reset the (WEN) Write Enable Latch bit.
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_WriteDisable(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the write disable command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_DISABLE_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Read Flash Status register value
  * @param  Ctx Component object pointer
  * @param  Value Status register value pointer
  * @retval error status
  */
int32_t MX25R6435F_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of status register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_STATUS_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 1U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Write Flash Status register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to Status register
  * @retval error status
  */
int32_t MX25R6435F_WriteStatusRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg[3];

  /* Status register is configured with configuration register 1 and 2 */
  if (MX25R6435F_ReadCfgRegister(Ctx, &reg[1]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  if (MX25R6435F_ReadCfg2Register(Ctx, &reg[2]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  reg[0] = Value;

  /* Initialize the writing of status register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_STATUS_CFG_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 3U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Write Flash configuration register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to configuration register
  * @retval error status
  */
int32_t MX25R6435F_WriteCfgRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg[3];

  /* Configuration register is configured with configuration register 2 and status register */
  if (MX25R6435F_ReadStatusRegister(Ctx, &reg[0]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  if (MX25R6435F_ReadCfg2Register(Ctx, &reg[2]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  reg[1] = Value;
  
  /* Initialize the writing of configuration register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_STATUS_CFG_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 3U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Read Flash configuration register value
  * @param  Ctx Component object pointer
  * @param  Value configuration register value pointer
  * @retval error status
  */
int32_t MX25R6435F_ReadCfgRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg[2];

  /* Initialize the reading of configuration register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_CFG_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 2U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }
  
  *Value = reg[0];

  return MX25R6435F_OK;
}

/**
  * @brief  Write Flash configuration register 2
  * @param  Ctx Component object pointer
  * @param  Value Value to write to configuration register
  * @retval error status
  */
int32_t MX25R6435F_WriteCfg2Register(QSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg[3];

  /* Configuration register 2 is configured with configuration register 1 and status register */
  if (MX25R6435F_ReadStatusRegister(Ctx, &reg[0]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  if (MX25R6435F_ReadCfg2Register(Ctx, &reg[1]) != MX25R6435F_OK)
  {
    return MX25R6435F_ERROR;
  }

  reg[2] = Value;

  /* Initialize the writing of configuration register 2 */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_STATUS_CFG_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 3U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Read Flash configuration register 2 value
  * @param  Ctx Component object pointer
  * @param  Value configuration register 2 value pointer
  * @retval error status
  */
int32_t MX25R6435F_ReadCfg2Register(QSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg[2];

  /* Initialize the reading of status register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_CFG_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 2U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  *Value = reg[1];

  return MX25R6435F_OK;
}

/**
  * @brief  Write Flash Security register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to Security register
  * @retval error status
  */
int32_t MX25R6435F_WriteSecurityRegister(QSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the write of security register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_WRITE_SEC_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 1U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, &Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Read Flash Security register value
  * @param  Ctx Component object pointer
  * @param  Value Security register value pointer
  * @retval error status
  */
int32_t MX25R6435F_ReadSecurityRegister(QSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of security register */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_SEC_REG_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 1U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}


/* ID Commands ****************************************************************/
/**
  * @brief  Read Flash 3 Byte IDs.
  *         Manufacturer ID, Memory type, Memory density
  * @param  Ctx Component object pointer
  * @param  ID 3 bytes IDs pointer
  * @retval error status
  */
int32_t MX25R6435F_ReadID(QSPI_HandleTypeDef *Ctx, uint8_t *ID)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_READ_ID_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_1_LINE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = 3U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, ID, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/* Reset Commands *************************************************************/
/**
  * @brief  Flash reset enable command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_ResetEnable(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_RESET_ENABLE_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Flash reset memory command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_ResetMemory(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_RESET_MEMORY_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Flash no operation command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_NoOperation(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the no operation command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_NO_OPERATION_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

/**
  * @brief  Flash enter deep power-down command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R6435F_EnterPowerDown(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the enter power down command */
  s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MX25R6435F_DEEP_POWER_DOWN_CMD;
  s_command.AddressMode        = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = QSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DdrMode            = QSPI_DDR_MODE_DISABLE;
#if defined(QUADSPI_CCR_DHHC) || defined(QSPI_DDR_HHC_ANALOG_DELAY)
  s_command.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
#endif
  s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R6435F_ERROR;
  }

  return MX25R6435F_OK;
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
