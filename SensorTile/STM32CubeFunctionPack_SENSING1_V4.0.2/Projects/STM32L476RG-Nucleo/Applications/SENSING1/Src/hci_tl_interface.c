/**
  ******************************************************************************
  * @file  hci_tl_interface.c
  * @author  SRA - Central Labs
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief  This file provides the implementation for all functions prototypes 
  *          for the STM32 BlueNRG HCI Transport Layer interface
  ******************************************************************************
  *
  * COPYRIGHT 2018 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#define HCI_TL
#define HCI_TL_INTERFACE

#ifdef HCI_TL
#include "hci_tl.h"
#else
#include "hci_tl_interface.h"
#endif /* HCI_TL */

/* Defines -------------------------------------------------------------------*/

#define HEADER_SIZE       5U
#define MAX_BUFFER_SIZE   255U
#define TIMEOUT_DURATION  15U


/* Private function prototypes -----------------------------------------------*/
#ifdef SENSING1_BlueNRG2
static void HCI_TL_SPI_Enable_IRQ(void);
static void HCI_TL_SPI_Disable_IRQ(void);
static int32_t IsDataAvailable(void);
#endif /* SENSING1_BlueNRG2 */

/******************** IO Operation and BUS services ***************************/
#ifdef SENSING1_BlueNRG2
/**
 * @brief  Enable SPI IRQ.
 * @param  None
 * @retval None
 */
static void HCI_TL_SPI_Enable_IRQ(void)
{
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);  
}

/**
 * @brief  Disable SPI IRQ.
 * @param  None
 * @retval None
 */
static void HCI_TL_SPI_Disable_IRQ(void)
{ 
  HAL_NVIC_DisableIRQ(HCI_TL_SPI_EXTI_IRQn);
}
#endif /* SENSING1_BlueNRG2 */

/**
 * @brief  Initializes the peripherals communication with the BlueNRG
 *         Expansion Board (via SPI, I2C, USART, ...)
 *
 * @param  void* Pointer to configuration struct 
 * @retval int32_t Status
 */
int32_t HCI_TL_SPI_Init(void* pConf)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
#ifdef USE_STM32L4XX_NUCLEO

  __HAL_RCC_GPIOA_CLK_ENABLE();

#elif defined(STM32_SENSORTILE)

   HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);

#elif defined(USE_STM32L475E_IOT01)

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

#elif defined(STM32_SENSORTILEBOX)

    HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


#else 
#error "Define the right platform"
#endif /* STM32_SENSORTILE */
  
  /*Configure EXTI Line */
  GPIO_InitStruct.Pin = HCI_TL_SPI_EXTI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HCI_TL_SPI_EXTI_PORT, &GPIO_InitStruct);
   
  /*Configure CS & RESET Line */
  GPIO_InitStruct.Pin =  HCI_TL_RST_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HCI_TL_RST_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = HCI_TL_SPI_CS_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HCI_TL_SPI_CS_PORT, &GPIO_InitStruct);

  return BSP_SPI_Init();
}

/**
 * @brief  DeInitializes the peripherals communication with the BlueNRG
 *         Expansion Board (via SPI, I2C, USART, ...)
 *
 * @param  None
 * @retval int32_t 0
 */
int32_t HCI_TL_SPI_DeInit(void)
{
  HAL_GPIO_DeInit(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN); 
  HAL_GPIO_DeInit(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN); 
  HAL_GPIO_DeInit(HCI_TL_RST_PORT, HCI_TL_RST_PIN);   
  return 0;
}

/**
 * @brief Reset BlueNRG module.
 *
 * @param  None
 * @retval int32_t 0
 */
int32_t HCI_TL_SPI_Reset(void)
{
#ifdef SENSING1_BlueNRG2
  // Deselect CS PIN for BlueNRG to avoid spurious commands
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);
#endif /* SENSING1_BlueNRG2 */

  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(5);    
  return 0;
}  

/**
 * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
 *
 * @param  buffer : Buffer where data from SPI are stored
 * @param  size   : Buffer size
 * @retval int32_t: Number of read bytes
 */
int32_t HCI_TL_SPI_Receive(uint8_t* buffer, uint16_t size)
{
  uint16_t byte_count;
  uint8_t len = 0;
  uint8_t char_ff = 0xff;
  volatile uint8_t read_char;

  uint8_t header_master[HEADER_SIZE] = {0x0b, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];

#ifdef SENSING1_BlueNRG2  
  HCI_TL_SPI_Disable_IRQ();
#endif /* SENSING1_BlueNRG2 */

  /* CS reset */
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_RESET);

  /* Read the header */  
  BSP_SPI_SendRecv(header_master, header_slave, HEADER_SIZE);

#ifndef SENSING1_BlueNRG2
  if(header_slave[0] == 0x02)
#endif /* SENSING1_BlueNRG2 */
  {
    /* device is ready */
    byte_count = (header_slave[4] << 8)| header_slave[3];
  
    if(byte_count > 0) {
  
      /* avoid to read more data that size of the buffer */

      if (byte_count > size){
        byte_count = size;
      }        
  
      for(len = 0; len < byte_count; len++)
      {                                               
        BSP_SPI_SendRecv(&char_ff, (uint8_t*)&read_char, 1);  
        buffer[len] = read_char;
      }      
    }    
  }
  /* Release CS line */
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);

#ifdef SENSING1_BlueNRG2
  HCI_TL_SPI_Enable_IRQ();
#endif /* SENSING1_BlueNRG2 */
  
#if PRINT_CSV_FORMAT
  if (len > 0) {
#ifdef SENSING1_BlueNRG2
    PRINT_CSV("BTOH->>\n");
#endif /* SENSING1_BlueNRG2 */
    print_csv_time();
    for (int i=0; i<len; i++) {
      PRINT_CSV(" %02x", buffer[i]);
    }
    PRINT_CSV("\n");
  }
#endif
  
  return len;  
}

/**
 * @brief  Writes data from local buffer to SPI.
 *
 * @param  buffer : data buffer to be written
 * @param  size   : size of first data buffer to be written
 * @retval int32_t: Number of read bytes
 */
int32_t HCI_TL_SPI_Send(uint8_t* buffer, uint16_t size)
{
#ifdef SENSING1_BlueNRG2
  #if PRINT_CSV_FORMAT
    PRINT_CSV("HTOB->>\n");
    print_csv_time();
    for (int i=0; i<size; i++)
    {
      PRINT_CSV(" %02x", buffer[i]);
    }
    PRINT_CSV("\n");
  #endif

  uint16_t rx_bytes;
#endif /* SENSING1_BlueNRG2 */

  int32_t result;  
  
  uint8_t header_master[HEADER_SIZE] = {0x0a, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];
  
  static uint8_t read_char_buf[MAX_BUFFER_SIZE];
  uint32_t tickstart = HAL_GetTick();

#ifdef SENSING1_BlueNRG2  
  HCI_TL_SPI_Disable_IRQ();
#endif /* SENSING1_BlueNRG2 */

  do
  {
#ifdef SENSING1_BlueNRG2
    uint32_t tickstart_data_available = HAL_GetTick();
#endif /* SENSING1_BlueNRG2 */
    
    result = 0;
    
    /* CS reset */
    HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_RESET);

#ifdef SENSING1_BlueNRG2
    /*
     * Wait until BlueNRG-2 is ready.
     * When ready it will raise the IRQ pin.
     */
    while(!IsDataAvailable())
    {
      if((HAL_GetTick() - tickstart_data_available) > TIMEOUT_DURATION)
      {
        result = -3;
        break;
      }
    }
    if(result == -3)
    {
      break;
    }
#endif /* SENSING1_BlueNRG2 */
    
    /* Read header */  
    BSP_SPI_SendRecv(header_master, header_slave, HEADER_SIZE);

#ifdef SENSING1_BlueNRG2    
    rx_bytes = (((uint16_t)header_slave[2])<<8) | ((uint16_t)header_slave[1]);

    if(rx_bytes >= size)
    {
      /* Buffer is big enough */
#else /* SENSING1_BlueNRG2 */
    
    if(header_slave[0] == 0x02) 
    {
      /* SPI is ready */
      if(header_slave[1] >= size) 
      {
#endif /* SENSING1_BlueNRG2 */
        BSP_SPI_SendRecv(buffer, read_char_buf, size);
      } 
      else 
      {
        /* Buffer is too small */
        result = -2;
      }
#ifndef SENSING1_BlueNRG2
    } else {
      /* SPI is not ready */
      result = -1;
    }
#endif /* SENSING1_BlueNRG2 */
    
    /* Release CS line */
    HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);
    
    if((HAL_GetTick() - tickstart) > TIMEOUT_DURATION)
    {
      result = -3;
      break;
    }
  } while(result < 0);
  
#ifdef SENSING1_BlueNRG2
  HCI_TL_SPI_Enable_IRQ();
#endif /* SENSING1_BlueNRG2 */
  
  return result;
}

#ifdef HCI_TL
/**
 * @brief  Reports if the BlueNRG has data for the host micro.
 *
 * @param  None
 * @retval int32_t: 1 if data are present, 0 otherwise
 */
static int32_t IsDataAvailable(void)
{
  return (HAL_GPIO_ReadPin(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN) == GPIO_PIN_SET);
} 
#endif /* HCI_TL */

/***************************** hci_tl_interface main functions *****************************/
/**
 * @brief  Register hci_tl_interface IO bus services
 *
 * @param  None
 * @retval None
 */ 
void hci_tl_lowlevel_init(void)
{
  /* USER CODE BEGIN hci_tl_lowlevel_init 1 */
  
  /* USER CODE END hci_tl_lowlevel_init 1 */
#ifdef HCI_TL
  tHciIO fops;  
  
  /* Register IO bus services */
  fops.Init    = HCI_TL_SPI_Init;
  fops.DeInit  = HCI_TL_SPI_DeInit;
  fops.Send    = HCI_TL_SPI_Send;
  fops.Receive = HCI_TL_SPI_Receive;
  fops.Reset   = HCI_TL_SPI_Reset;
  fops.GetTick = BSP_GetTick;
  
  hci_register_io_bus (&fops);
  
  /* USER CODE BEGIN hci_tl_lowlevel_init 2 */
  
  /* USER CODE END hci_tl_lowlevel_init 2 */
  
  /* Register event irq handler */
  #ifdef STM32_SENSORTILE

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  #elif defined(USE_STM32L4XX_NUCLEO)

    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  #elif defined(USE_STM32L475E_IOT01)

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  #elif defined(STM32_SENSORTILEBOX)

    HAL_NVIC_SetPriority(HCI_TL_SPI_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);

  #else /* STM32_SENSORTILE */
    #error "Define the right platform"
  #endif /* STM32_SENSORTILE */
#endif /* HCI_TL */  

  /* USER CODE BEGIN hci_tl_lowlevel_init 3 */
  
  /* USER CODE END hci_tl_lowlevel_init 3 */

}

/**
  * @brief HCI Transport Layer Low Level Interrupt Service Routine
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_isr(void)
{
  /* Call hci_notify_asynch_evt() */
#ifdef HCI_TL
  while(IsDataAvailable())
  {
#ifdef SENSING1_BlueNRG2
    hci_notify_asynch_evt(NULL);
#else /* SENSING1_BlueNRG2 */
    if (hci_notify_asynch_evt(NULL))
    {
      return;
    }
#endif /* SENSING1_BlueNRG2 */
  }
#endif /* HCI_TL */

  /* USER CODE BEGIN hci_tl_lowlevel_isr */

  /* USER CODE END hci_tl_lowlevel_isr */ 
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
