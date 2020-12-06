/**
  ******************************************************************************
  * @file    TargetPlatform.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Initialization of the Target Platform
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
#include <stdio.h>
#include "TargetFeatures.h"
#include "main.h"

#if SENSING1_USE_DATALOG
#include "DataLog_Manager.h"
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_USB_CDC
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
#endif /* SENSING1_USE_USB_CDC */

#if SENSING1_USE_USB_MSC
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage_if.h"
#endif /* SENSING1_USE_USB_MSC */

#if SENSING1_USE_USB_AUDIO
#include "usbd_desc.h"
#include "usbd_audio_if.h"
#endif /* SENSING1_USE_USB_AUDIO */

/* Imported variables ---------------------------------------------------------*/
#if SENSING1_USE_USB_CDC
  extern USBD_DescriptorsTypeDef USBD_Desc;
#endif /* SENSING1_USE_USB_CDC */

#if SENSING1_USE_USB_AUDIO
extern USBD_DescriptorsTypeDef USBD_Desc;
extern USBD_AUDIO_ItfTypeDef  USBD_AUDIO_fops;
#endif /* SENSING1_USE_USB_AUDIO */

/* Exported variables ---------------------------------------------------------*/
TargetFeatures_t TargetBoardFeatures;

#if SENSING1_USE_USB
USBD_HandleTypeDef  hUSBDevice;
#endif /* SENSING1_USE_USB */

volatile float RMS_Ch[AUDIO_CHANNELS];
float DBNOISE_Value_Old_Ch[AUDIO_CHANNELS];
#ifdef USE_STM32L475E_IOT01
uint16_t PCM_Buffer[PCM_BUFFER_LEN];
#else
uint16_t PCM_Buffer[AUDIO_CHANNELS*PCM_AUDIO_IN_SAMPLES];
#endif
BSP_AUDIO_Init_t MicParams;

/* Local defines -------------------------------------------------------------*/

/* Local function prototypes --------------------------------------------------*/
static void Init_MEM1_Sensors(void);
static void Init_MEMS_Mics(uint32_t AudioFreq);

/**
  * @brief  Initialize all the Target platform's Features
  * @param  TargetType_t BoardType Nucleo/BlueCoin/SensorTile
  * @retval None
  */
void InitTargetPlatform(TargetType_t BoardType)
{
  TargetBoardFeatures.BoardType = BoardType;

#ifdef USE_STM32L4XX_NUCLEO

  #if SENSING1_USE_PRINTF
    /* UART Initialization */
    if(UART_Global_Init()!=HAL_OK) {
      Error_Handler();
    } else {
      SENSING1_PRINTF("UART Initialized\r\n");
    }
  #endif /* SENSING1_USE_PRINTF */

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Initialize LED */
  LedInitTargetPlatform();

#elif defined(STM32_SENSORTILE)

  #if SENSING1_USE_USB_CDC

  InitUSBCDC();

  /* 10 seconds ... for having time to open the Terminal
   * for looking the SENSING1 Initialization phase */
  HAL_Delay(10000);

  #endif /* SENSING1_USE_USB_CDC */

  /* Initialize LED */
  LedInitTargetPlatform();

#elif defined(USE_STM32L475E_IOT01)

  /* Initialize button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  #if SENSING1_USE_PRINTF
  /* UART Initialization */
  COM_InitTypeDef COM_InitStruct;

  COM_InitStruct.BaudRate   = CFG_HW_UART1_BAUDRATE;
  COM_InitStruct.WordLength = (COM_WordLengthTypeDef) CFG_HW_UART1_WORDLENGTH;
  COM_InitStruct.StopBits   = COM_STOPBITS_1;
  COM_InitStruct.Parity     = COM_PARITY_NONE;
  COM_InitStruct.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &COM_InitStruct) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  else
  {
    SENSING1_PRINTF("UART Initialized\r\n");
  }
  #endif /* SENSING1_USE_PRINTF */

  /* Initialize LED */
  LedInitTargetPlatform();

#if SENSING1_USE_USB_MSC
  /* Enable USB Mass Storage Device if USER button is pressed */
  if (BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_SET)
  {
    InitUSBMSC();
    /* Don't start anything else and stay here */
    while(1);
  }
#endif /* SENSING1_USE_USB_MSC */

#elif defined(STM32_SENSORTILEBOX)

  /* Init Led1/Led2 */
  LedInitTargetPlatform();

    /* Initialize User Button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Initialize the Power Button */
  BSP_PowerButton_Init();

  /* Initialize the Battery Charger */
  BSP_BC_Init();

  /* In order to be able to Read Battery Volt */
  BSP_BC_BatMS_Init();

#else
  #error "Define the right platform"  
#endif /* USE_STM32L4XX_NUCLEO */

  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t));
  /* Discovery and Intialize all the MEMS Target's Features */
  Init_MEM1_Sensors();
  /* Disable all the MEMS Target's Features */
  DisableMotionSensors();
  DisableEnvSensors();

#ifdef STM32_SENSORTILE
  /* Initialize the Gas Gauge if the battery is present */
  if(BSP_GG_Init(&TargetBoardFeatures.HandleGGComponent) == COMPONENT_OK){
    SENSING1_PRINTF("OK Gas Gauge Component\r\n");
  } else {
    SENSING1_PRINTF("Battery not present\r\n");
  }
#elif STM32_SENSORTILEBOX
  /* Just for putting the Handle != Null */
  TargetBoardFeatures.HandleGGComponent=(void*) 0xDEADBEEF;
#endif /* STM32_SENSORTILE */

  /* Default Microphones' Audio Volume */
  TargetBoardFeatures.AudioVolume = AUDIO_VOLUME_VALUE;

#ifdef STM32_SENSORTILEBOX
  MCR_HEART_BIT();
  MCR_HEART_BIT();
#endif /* STM32_SENSORTILEBOX */

#if SENSING1_USE_DATALOG
  /* Initialize the local filesystem and volume driver */
  DATALOG_SD_Init();
  HAL_Delay(200);
#if defined(STM32_SENSORTILE)
  sprintf(DefaultDataFileName,"%s","STile");
#elif defined(USE_STM32L475E_IOT01)
  sprintf(DefaultDataFileName,"%s","IoT01");
#elif defined(STM32_SENSORTILEBOX)
  sprintf(DefaultDataFileName,"%s","STBox");
#else /* STM32_SENSORTILE */
  #error "Add something for this platform"
#endif /* STM32_SENSORTILE */

#endif /* SENSING1_USE_DATALOG */

  SENSING1_PRINTF("\n\r");
}

/** @brief enable all the Inertial MEMS1 sensors
 * @param None
 * @retval None
 */
void EnableMotionSensors (void)
{
  if(TargetBoardFeatures.HandleAccSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Enabled Accelero Sensor\r\n");
    }
  }

  if(TargetBoardFeatures.HandleGyroSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Enable(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Enabled Gyroscope Sensor\r\n");
    }
  }

  if(TargetBoardFeatures.HandleMagSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Enable(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Enabled Magneto Sensor\r\n");
    }
  }
}

/** @brief disable all the Inertial MEMS1 sensors
 * @param None
 * @retval None
 */
void DisableMotionSensors (void)
{
  if(TargetBoardFeatures.HandleAccSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Disable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Accelero Sensor\r\n");
    }
  }

  if(TargetBoardFeatures.HandleGyroSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Disable(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Gyroscope Sensor\r\n");
    }
  }

  if(TargetBoardFeatures.HandleMagSensor != SENSING1_SNS_NOT_VALID) {
    if(MOTION_SENSOR_Disable(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Magneto Sensor\r\n");
    }
  }
}

/** @brief enable all the Environmental MEMS1 sensors
 * @param None
 * @retval None
 */
void EnableEnvSensors (void)
{
  if(TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    if(ENV_SENSOR_Enable(TargetBoardFeatures.HandleHumSensor, ENV_HUMIDITY)==BSP_ERROR_NONE) {
#ifdef ONE_SHOT
      SENSING1_PRINTF("Enabled Humidity Sensor (One Shot)\r\n");
#else /* ONE_SHOT */
      SENSING1_PRINTF("Enabled Humidity Sensor\r\n");
#endif /* ONE_SHOT */
      if(ENV_SENSOR_Enable(TargetBoardFeatures.HandleTempSensors[0], ENV_TEMPERATURE)==BSP_ERROR_NONE) {
#ifdef ONE_SHOT
        SENSING1_PRINTF("Enabled Temperature Sensor1 (One Shot)\r\n");
#else /* ONE_SHOT */
        SENSING1_PRINTF("Enabled Temperature Sensor1\r\n");
#endif /* ONE_SHOT */
      }
    }
  }

  if(TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    if(ENV_SENSOR_Enable(TargetBoardFeatures.HandlePressSensor, ENV_PRESSURE)==BSP_ERROR_NONE) {
#ifdef ONE_SHOT
      SENSING1_PRINTF("Enabled Pressure Sensor (One Shot)\r\n");
#else /* ONE_SHOT */
      SENSING1_PRINTF("Enabled Pressure Sensor\r\n");
#endif /* ONE_SHOT */
      if(ENV_SENSOR_Enable(TargetBoardFeatures.HandleTempSensors[1], ENV_TEMPERATURE)==BSP_ERROR_NONE) {
#ifdef ONE_SHOT
        SENSING1_PRINTF("Enabled Temperature Sensor2 (One Shot)\r\n");
#else /* ONE_SHOT */
        SENSING1_PRINTF("Enabled Temperature Sensor2\r\n");
#endif /* ONE_SHOT */
      }
    }
  }

  TargetBoardFeatures.EnvSensorEnabled= 1;
}

/** @brief disable all the Environmental MEMS1 sensors
 * @param None
 * @retval None
 */
void DisableEnvSensors (void)
{
  if(TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    if(ENV_SENSOR_Disable(TargetBoardFeatures.HandleHumSensor, ENV_HUMIDITY)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Humidity Sensor\r\n");
    }
    if(ENV_SENSOR_Disable(TargetBoardFeatures.HandleTempSensors[0], ENV_TEMPERATURE)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Temperature Sensor1\r\n");
    }
  }

  if(TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    if(ENV_SENSOR_Disable(TargetBoardFeatures.HandlePressSensor, ENV_PRESSURE)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Pressure Sensor\r\n");
    }
    if(ENV_SENSOR_Disable(TargetBoardFeatures.HandleTempSensors[1], ENV_TEMPERATURE)==BSP_ERROR_NONE) {
      SENSING1_PRINTF("Disabled Temperature Sensor2\r\n");
    }
  }

  TargetBoardFeatures.EnvSensorEnabled= 0;
}

/** @brief Initialize all the MEMS1 sensors
 * @param None
 * @retval None
 */
static void Init_MEM1_Sensors(void)
{
#if defined(STM32_SENSORTILE)

  /* Handles for SensorTile board */
  TargetBoardFeatures.HandleAccSensor  = LSM6DSM_0;
  TargetBoardFeatures.HandleGyroSensor = LSM6DSM_0;
  TargetBoardFeatures.HandleMagSensor  = LSM303AGR_MAG_0;

  TargetBoardFeatures.HandleHumSensor      = HTS221_0;
  TargetBoardFeatures.HandleTempSensors[0] = HTS221_0;

  TargetBoardFeatures.HandlePressSensor    = LPS22HB_0;
  TargetBoardFeatures.HandleTempSensors[1] = LPS22HB_0;

#elif defined(USE_STM32L4XX_NUCLEO)

  /* Handles for X-NUCLEO-IKS01A2 expansion board */
  TargetBoardFeatures.HandleAccSensor  = IKS01A2_LSM6DSL_0;
  TargetBoardFeatures.HandleGyroSensor = IKS01A2_LSM6DSL_0;
  TargetBoardFeatures.HandleMagSensor  = IKS01A2_LSM303AGR_MAG_0;

  TargetBoardFeatures.HandleHumSensor      = IKS01A2_HTS221_0;
  TargetBoardFeatures.HandleTempSensors[0] = IKS01A2_HTS221_0;

  TargetBoardFeatures.HandlePressSensor    = IKS01A2_LPS22HB_0;
  TargetBoardFeatures.HandleTempSensors[1] = IKS01A2_LPS22HB_0;

#elif defined(USE_STM32L475E_IOT01)

  /* Handles for IOT01A1 board */
  TargetBoardFeatures.HandleAccSensor  = 0;
  TargetBoardFeatures.HandleGyroSensor = 0;
  TargetBoardFeatures.HandleMagSensor  = 1;

  TargetBoardFeatures.HandleHumSensor      = 0;
  TargetBoardFeatures.HandleTempSensors[0] = 0;

  TargetBoardFeatures.HandlePressSensor    = 1;
  TargetBoardFeatures.HandleTempSensors[1] = 1;

#elif defined(STM32_SENSORTILEBOX)

  /* Handles for SensorTile.box board */
  TargetBoardFeatures.HandleAccSensor  = LSM6DSOX_0;
  TargetBoardFeatures.HandleGyroSensor = LSM6DSOX_0;
  TargetBoardFeatures.HandleMagSensor  = LIS2MDL_0;

  TargetBoardFeatures.HandleHumSensor      = HTS221_0;
  TargetBoardFeatures.HandleTempSensors[0] = HTS221_0;

  TargetBoardFeatures.HandlePressSensor    = LPS22HH_0;
  TargetBoardFeatures.HandleTempSensors[1] = LPS22HH_0;

#else
  #error "Write Something here"
#endif /* STM32_SENSORTILE */
  /* Accelero/Gyro It's necessary to Init the Component ONLY one Time */
  if (MOTION_SENSOR_Init(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO|MOTION_GYRO) == BSP_ERROR_NONE){
    SENSING1_PRINTF("OK Accelero/Gyroscope Sensor\n\r");
  } else {
    SENSING1_PRINTF("Error Accelero/Gyroscope Sensor\n\r");
    TargetBoardFeatures.HandleAccSensor  = SENSING1_SNS_NOT_VALID;
    TargetBoardFeatures.HandleGyroSensor = SENSING1_SNS_NOT_VALID;
    /* if the Acc is missing... it's blocking for this application !! */
    while(1);
  }

  /* set default range to 2G */
  Set2GAccelerometerFullScale();

#ifndef USE_STM32L475E_IOT01
  /* For accelero HW features */
  InitHWFeatures();
#endif /* USE_STM32L475E_IOT01 */

  if(MOTION_SENSOR_Init(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO)==BSP_ERROR_NONE){
    SENSING1_PRINTF("OK Magneto Sensor\n\r");
  } else {
    SENSING1_PRINTF("Error Magneto Sensor\n\r");
    TargetBoardFeatures.HandleMagSensor = SENSING1_SNS_NOT_VALID;
  }

  /* Humidity/Temperature1 It's necessary to Init the Component ONLY one Time */
  if(ENV_SENSOR_Init(TargetBoardFeatures.HandleHumSensor, ENV_HUMIDITY|ENV_TEMPERATURE)==BSP_ERROR_NONE){
    SENSING1_PRINTF("OK Humidity/Temperature1 Sensor\n\r");
    TargetBoardFeatures.NumTempSensors++;
  } else {
    SENSING1_PRINTF("Error Humidity/Temperature1 Sensor\n\r");
    TargetBoardFeatures.HandleHumSensor      = SENSING1_SNS_NOT_VALID;
    TargetBoardFeatures.HandleTempSensors[0] = SENSING1_SNS_NOT_VALID;
  }

  /* Pressure/Temperature 2 It's necessary to Init the Component ONLY one Time */
  if(ENV_SENSOR_Init(TargetBoardFeatures.HandlePressSensor, ENV_PRESSURE|ENV_TEMPERATURE)==BSP_ERROR_NONE){
    SENSING1_PRINTF("OK Pressure/Temperature2 Sensor\n\r");
    TargetBoardFeatures.NumTempSensors++;
  } else {
    SENSING1_PRINTF("Error Pressure/Temperature2 Sensor\n\r");
    TargetBoardFeatures.HandlePressSensor    = SENSING1_SNS_NOT_VALID;
    TargetBoardFeatures.HandleTempSensors[1] = SENSING1_SNS_NOT_VALID;
  }

#ifndef USE_STM32L475E_IOT01
  /* The IoT01A1 has not the HW events */
  #if defined(USE_STM32L4XX_NUCLEO)
    /* Enable interruption LSM6DSL_INT2_PIN/INT1 */
    {
      GPIO_InitTypeDef GPIO_InitStruct;

      __HAL_RCC_GPIOB_CLK_ENABLE();

      GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(EXTI4_IRQn);

      HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
  #elif defined(STM32_SENSORTILE)
    /* Enable interruption LSM6DSM_INT2_PIN  */
    {
      GPIO_InitTypeDef GPIO_InitStruct;

      __HAL_RCC_GPIOA_CLK_ENABLE();

      GPIO_InitStruct.Pin = BSP_LSM6DSM_INT2;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      /* EXTI interrupt init*/
      HAL_NVIC_SetPriority(BSP_LSM6DSM_INT2_EXTI_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(BSP_LSM6DSM_INT2_EXTI_IRQn);
    }
  #elif defined(STM32_SENSORTILEBOX)
    /* Enable interruption LSM6DSOX  */
    {
      GPIO_InitTypeDef GPIO_InitStruct;

      __HAL_RCC_GPIOE_CLK_ENABLE();

      GPIO_InitStruct.Pin = GPIO_PIN_3;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

      /* EXTI interrupt init*/
      HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    }
  #else
    #error "Define the right platform"
  #endif /* USE_STM32L4XX_NUCLEO */
#endif /* USE_STM32L475E_IOT01 */
}

/** @brief Initialize all the MEMS's Microphones
 * @param None
 * @retval None
 */
static void Init_MEMS_Mics(uint32_t AudioFreq)
{
  uint8_t ret;

  MicParams.BitsPerSample = 16;
  MicParams.ChannelsNbr = AUDIO_CHANNELS;
#ifdef STM32_SENSORTILEBOX
  MicParams.Device = AMIC_ONBOARD;
#elif USE_STM32L475E_IOT01
  MicParams.Device = AUDIO_IN_DIGITAL_MIC1;
#else
  MicParams.Device = AUDIO_IN_DIGITAL_MIC;
#endif /* STM32_SENSORTILEBOX */
  MicParams.SampleRate = AudioFreq;
  MicParams.Volume = TargetBoardFeatures.AudioVolume;

  ret = BSP_AUDIO_IN_Init(AUDIO_INSTANCE, &MicParams);

  if(ret != BSP_ERROR_NONE) {
    SENSING1_PRINTF("\nError Audio Init\r\n");
    while(1) {
      ;
    }
  } else {
    SENSING1_PRINTF("\nOK Audio Init\t(Audio Freq.= %ld)\r\n", AudioFreq);
  }

#ifndef USE_STM32L475E_IOT01
  /* Set the volume level */
  ret= BSP_AUDIO_IN_SetVolume(AUDIO_INSTANCE,TargetBoardFeatures.AudioVolume);

  if(ret != BSP_ERROR_NONE) {
    SENSING1_PRINTF("Error Audio Volume\r\n\n");

    while(1) {
      ;
    }
  } else {
    SENSING1_PRINTF("OK Audio Volume\t(Volume= %ld)\r\n", TargetBoardFeatures.AudioVolume);
  }
#endif /* USE_STM32L475E_IOT01 */

  /* Number of Microphones */
  TargetBoardFeatures.NumMicSensors=AUDIO_CHANNELS;
}

/** @brief Initialize all the MEMS's Microphones
 * @param None
 * @retval None
 */
void InitMics(uint32_t AudioFreq)
{
  Init_MEMS_Mics(AudioFreq);

#if defined(USE_STM32L4XX_NUCLEO)
  BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, 0);
#elif defined(STM32_SENSORTILE)
  BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_AUDIO_IN_SAMPLES*2);
#elif defined(USE_STM32L475E_IOT01)
  BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_AUDIO_IN_SAMPLES*4);
#elif defined(STM32_SENSORTILEBOX)
  BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_AUDIO_IN_SAMPLES*2);
#else
  #error "Platform not supported"
#endif /* USE_STM32L4XX_NUCLEO */
}

/** @brief DeInitialize all the MEMS's Microphones
 * @param None
 * @retval None
 */
void DeInitMics(void)
{
  uint8_t ret = BSP_ERROR_NONE;

  BSP_AUDIO_IN_Stop(AUDIO_INSTANCE);

  /* The ADC could be used also for Battery Charger */
  ret = BSP_AUDIO_IN_DeInit(AUDIO_INSTANCE);

  if (ret != BSP_ERROR_NONE) {
    SENSING1_PRINTF("Error Audio DeInit\r\n");
    while(1);
  } else {
    SENSING1_PRINTF("OK Audio DeInit\r\n");
  }
}

#if SENSING1_USE_USB_AUDIO

/** @brief Initialize USB Audio
 * @param None
 * @retval None
 */
void InitUSBAudio(void)
{
  /* USB Initialization */
  /* Enable USB power */
  HAL_PWREx_EnableVddUSB();
  /* Init Device Library */
  USBD_Init(&hUSBDevice, &USBD_Desc, 0);
  /* Add Supported Class */
  USBD_RegisterClass(&hUSBDevice, &USBD_AUDIO);
  /* Add Interface callbacks for AUDIO Class */
  USBD_AUDIO_RegisterInterface(&hUSBDevice, &USBD_AUDIO_fops);
  /* Start USB*/
  USBD_Start(&hUSBDevice);

  SENSING1_PRINTF("\nOK USB Audio Init\t(Audio Freq.= %d)\r\n", USBD_AUDIO_FREQ);
}

/** @brief DeInitialize USB Audio
 * @param None
 * @retval None
 */
void DeInitUSBAudio(void)
{
  USBD_StatusTypeDef ret = USBD_FAIL;

  /* Stop USB */
  USBD_Stop(&hUSBDevice);

  /* DeInit Device Library */
  ret = USBD_DeInit(&hUSBDevice);

  if (ret != USBD_OK) {
    SENSING1_PRINTF("Error USB Audio DeInit\r\n");
    while(1);
  } else {
    SENSING1_PRINTF("OK USB Audio DeInit\r\n");
  }
}

#endif /* SENSING1_USE_USB_AUDIO */

#if SENSING1_USE_USB_MSC
/**
 * @brief  Initialize USB Mass Storage Device
 * @param  None
 * @retval None
 */
void InitUSBMSC(void)
{
  SENSING1_PRINTF("Starting USB Mass Storage Device mode\r\n");
  HAL_PWREx_EnableVddUSB();
  USBD_Init(&hUSBDevice, &USBD_Desc, 0);
  USBD_RegisterClass(&hUSBDevice, &USBD_MSC);
  USBD_MSC_RegisterStorage(&hUSBDevice, &USBD_Storage_Interface_fops_FS);
  USBD_Start(&hUSBDevice);
}

/**
 * @brief  DeInitialize USB Mass Storage Device
 * @param  None
 * @retval None
 */
void DeInitUSBMSC(void)
{
  /* Stop USB */
  USBD_Stop(&hUSBDevice);

  /* DeInit Device Library */
  USBD_DeInit(&hUSBDevice);
}
#endif /* SENSING1_USE_USB_CDC */

#if SENSING1_USE_USB_CDC

/**
 * @brief Initialize USB Communications Device Class
 * @param None
 * @retval None
 */
void InitUSBCDC(void)
{
  /* USB Initialization */
  /* Enable USB power */
  HAL_PWREx_EnableVddUSB();
  /* Init Device Library */
  USBD_Init(&hUSBDevice, &USBD_Desc, 0);
  /* Add Supported Class */
  USBD_RegisterClass(&hUSBDevice, &USBD_CDC);
  /* Add Interface callbacks for CDC Class */
  USBD_CDC_RegisterInterface(&hUSBDevice, &USBD_CDC_fops);
  /* Start USB*/
  USBD_Start(&hUSBDevice);
}

/**
 * @brief DeInitialize USB Audio
 * @param None
 * @retval None
 */
void DeInitUSBCDC(void)
{
  USBD_StatusTypeDef ret = USBD_FAIL;

  /* Stop USB */
  USBD_Stop(&hUSBDevice);

  /* DeInit Device Library */
  ret = USBD_DeInit(&hUSBDevice);

  if (ret != USBD_OK) {
    SENSING1_PRINTF("Error USB CDC DeInit\r\n");
    while(1);
  } else {
    SENSING1_PRINTF("OK USB CDC DeInit\r\n");
  }
}

#endif /* SENSING1_USE_USB_CDC */

/**
  * @brief  This function switches on the LED
  * @param  None
  * @retval None
  */
void LedOnTargetPlatform(void)
{
#if defined(USE_STM32L4XX_NUCLEO)
  BSP_LED_On(LED2);
#elif defined(STM32_SENSORTILE)
  BSP_LED_On( LED1 );
#elif defined(USE_STM32L475E_IOT01)
  BSP_LED_On(LED2);
#elif defined(STM32_SENSORTILEBOX)
  BSP_LED_On(LED1);
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */
}

/**
  * @brief  This function switches off the LED
  * @param  None
  * @retval None
  */
void LedOffTargetPlatform(void)
{
#if defined(USE_STM32L4XX_NUCLEO)
  BSP_LED_Off(LED2);
#elif defined(STM32_SENSORTILE)
  BSP_LED_Off(LED1);
#elif defined(USE_STM32L475E_IOT01)
  BSP_LED_Off(LED2);
#elif defined(STM32_SENSORTILEBOX)
  BSP_LED_Off(LED1);
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */
}

/** @brief  This function toggles the LED
  * @param  None
  * @retval None
  */
void LedToggleTargetPlatform(void)
{
#if defined(USE_STM32L4XX_NUCLEO)
  BSP_LED_Toggle(LED2);
#elif defined(STM32_SENSORTILE)
  BSP_LED_Toggle(LED1);
#elif defined(USE_STM32L475E_IOT01)
  BSP_LED_Toggle(LED2);
#elif defined (STM32_SENSORTILEBOX)
  BSP_LED_Toggle(LED1);
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */
}

/** @brief  This function initializes the LED
  * @param  None
  * @retval None
  */
void LedInitTargetPlatform(void)
{
#if defined(USE_STM32L4XX_NUCLEO)
  BSP_LED_Init(LED2);
#elif defined(STM32_SENSORTILE)
  BSP_LED_Init(LED1);
#elif defined(USE_STM32L475E_IOT01)
  BSP_LED_Init(LED2);
#elif defined(STM32_SENSORTILEBOX)
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t GetPage(uint32_t Addr)
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

/**
  * @brief  Gets the bank of a given address
  * @param  Addr Address of the FLASH Memory
  * @retval The bank of a given address
  */
uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
    /* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
      bank = FLASH_BANK_1;
    } else {
      bank = FLASH_BANK_2;
    }
  } else {
    /* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
      bank = FLASH_BANK_2;
    } else {
      bank = FLASH_BANK_1;
    }
  }

  return bank;
}

/**
 * @brief User function for Erasing the MDM on Flash
 * @param None
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForErasingFlash(void) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success=1;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = GetBank(MDM_FLASH_ADD);
  EraseInitStruct.Page        = GetPage(MDM_FLASH_ADD);
#ifndef STM32L4R9xx
  EraseInitStruct.NbPages     = 2; /* Each page is 2K */
#else /* STM32L4R9xx */
  EraseInitStruct.NbPages     = 1; /* Each page is 4k */
#endif /* STM32L4R9xx */

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
    Success=0;
    SENSING1_PRINTF("MetaDataManager Flash sector erase error\r\n");
    Error_Handler();
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/**
 * @brief User function for Saving the MDM  on the Flash
 * @param void * InitMetaDataVector Pointer to the MDM beginning
 * @param void * EndMetaDataVector Pointer to the MDM end
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForSavingFlash(void *InitMetaDataVector,void *EndMetaDataVector)
{
  uint32_t Success=1;

  /* Store in Flash Memory */
  uint32_t Address = MDM_FLASH_ADD;
  uint64_t *WriteIndex;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
  for(WriteIndex =((uint64_t *) InitMetaDataVector); WriteIndex<((uint64_t *) EndMetaDataVector); WriteIndex++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address,*WriteIndex) == HAL_OK){
      Address = Address + 8;
    } else {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      Error_Handler();
      Success =0;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
