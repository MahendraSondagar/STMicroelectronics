/**
  ******************************************************************************
  * @file    main.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Main program body
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

/**
 * @mainpage FP-AI-SENSING1 ultra-low power IoT node with Artificial Intelligence
 *
 * @image html st_logo.png
 *
 * <b>Introduction</b>
 *
 * This firmware package includes Components Device Drivers, Board Support Package
 * and example application for the following STMicroelectronics elements:
 * \if USE_STM32L4XX_NUCLEO
 * - X-NUCLEO-IDB05A1 Bluetooth Low energy expansion boards
 * - X-NUCLEO-IKS01A2 Expansion board for four MEMS sensor devices:
 *       HTS221, LPS22HB, LSM6DSL, LSM303AGR
 * - X-NUCLEO-CCA02M1 Digital MEMS microphones expansion board
 * \endif
 * \if STM32_SENSORTILE
 * - STEVAL-STLKT01V1 (SensorTile) evaluation board that contains the following components:
 *     - MEMS sensor devices: HTS221, LPS22HB, LSM303, LSM6DSM
 *     - digital microphone: MP34DT04
 *     - Gas Gauge IC with alarm output: STC3115
 * - FatFs generic FAT file system module provides access the storage devices
 *   such as memory card and hard disk.
 * \endif
 * \if STM32_SENSORTILE_BOX
 * - STEVAL-MKSBOX1V1 (SensorTile.box) evaluation board that contains the following components:
 *     - MEMS sensor devices: HTS221, LPS22HH, LIS2MDL, LSM6DSOX
 *     - analog microphone 
 * - FatFs generic FAT file system module provides access the storage devices
 *   such as memory card and hard disk.
 * \endif
 * \if USE_STM32L475E_IOT01
 * - B-L475E-IOT01A STM32L4 Discovery kit IoT node that contains the following components:
 *     - MEMS sensor devices: HTS221, LPS22HB, lIS3MDL, LSM6DSM
 *     - digital microphone: MP34DT04
 * - FatFs generic FAT file system module provides access the storage devices
 *   such as memory card and hard disk.
 * \endif
 * - FreeRTOS Real Time Kernel/Scheduler that allows applications to be organized as a collection of independent threads of execution
 *   (under MIT open source license)
 *
 * - Middleware libraries generated thanks to STM32CubeMx extension called X-CUBE-AI featuring example implementation of neural networks for real-time:
 *	  - human activity recognition (HAR)
 *	  - acoustic scene classification (ASC)
 *
 * \if USE_STM32L4XX_NUCLEO
 * @attention
 * <b>Important Hardware Additional Information</b><br>
 * <br>\image html X-NUCLEO-IKS01A2_HW_Info.jpg "Figure 1: X-NUCLEO-IKS01A2 expansion board"
 * <br>Before to connect X-NUCLEO-IKS01A2 with X-NUCLEO-CCA02M1 expansion board through the Arduino UNO R3 extension connector
 * on to X-NUCLEO-IKS01A2 board remove SB25 0-ohm resistor
 * 
 * <br>\image html X-NUCLEO-CCA02M1_HW_Info.jpg "Figure 2: X-NUCLEO-CCA02M1 expansion board"
 * <br>Before to connect the board X-NUCLEO-CCA02M1 with the STM32 L4 Nucleo motherboard through the Morpho connector layout,
 * on to X-NUCLEO-CCA02M1 board, close the solder bridges SB12, SB16 and open the solder bridges SB7, SB15 and SB17
 * \endif
 *
 * <b>Example Application</b>
 *
 * The Example application initializes all the Components and Library creating some Custom Bluetooth services:
 * - The first service exposes all the HW and SW characteristics:
 *  - HW characteristics:
 *      - related to MEMS sensor devices: Temperature, Humidity, Pressure, Magnetometer, Gyroscope and Accelerometer
 *        and Microphones Signal Noise dB level.
 * \if STM32_SENSORTILE
 *      - Battery status and SD Data Log control
 * \endif
 * \if STM32_SENSORTILE_BOX
 *      - Battery status and SD Data Log control
 * \endif
 * \if USE_STM32L475E_IOT01
 *      - SD Data Log control
 * \endif
 *      - human activity recognition (HAR) or acoustic scene classification (ASC)
 *
 * - The second Service exposes the console services where we have stdin/stdout and stderr capabilities
 * - The last Service is used for configuration purpose
 *
 * \if USE_STM32L4XX_NUCLEO
 * The example application allows the user to monitor the initialization phase via UART.
 * Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 * \elseif STM32_SENSORTILE
 * To enable UART monitoring, it is necessary to recompile the code with:
 * >  #define SENSING1_USE_PRINTF    1
 * on file:
 *   Inc\SENSING1_config.h
 *
 * This enables the UART that starts with a delay of 10 Seconds for allowing the time to open the UART for looking
 * the initialization phase launching a terminal application and setting the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 * \elseif USE_STM32L475E_IOT01
 * The example application allows the user to monitor the initialization phase via UART.
 * Launch a terminal application and set the UART port to 115200 bps, 8 bit, No Parity, 1 stop bit.
 * \endif
 *
 * This example must be used with the related BlueMS Android/iOS application available on Play/itune store (Version 4.1.0 or higher),
 * in order to read the sent information by Bluetooth Low Energy protocol
 *
 *                              --------------------
 *                              |  VERY IMPORTANT  |
 *                              --------------------
 * 4) For each IDE (IAR/µVision/System Workbench), and for each platform,
 * there are some scripts *.bat and *.sh that makes the following operations:
 * - Full Flash Erase
 * - Load the BootLoader on the right flash region
 * - Load the Program (after the compilation) on the right flash region
 * - Reset the board
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "TargetFeatures.h"
#include "cmsis_os.h"
#include "arm_math.h"
#include "main.h"
#include "cli_commands.h"


#include "PowerControl.h"
#include "bluenrg_conf.h"

#if SENSING1_USE_DATALOG
#include "DataLog_Manager.h"
#endif /* SENSING1_USE_DATALOG */

#include "har_Processing.h"
#include "ai_common.h"

#include "OTA.h"

#include "asc.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define LED_TIME_OFF 2000UL /* shall not be same than ON time */
#define LED_TIME_ON   100UL
#define ESC          (0x1B)
/* Imported Variables --------------------------------------------------------*/
extern uint8_t set_connectable;
extern volatile float RMS_Ch[];
extern float DBNOISE_Value_Old_Ch[];
extern uint16_t PCM_Buffer[];

#if defined (__ICCARM__) /*!< IAR Compiler */
  #pragma data_alignment = 4
#endif

float32_t Proc_Buffer_f[FILL_BUFFER_SIZE];
int16_t Fill_Buffer[FILL_BUFFER_SIZE];

/* Exported Variables --------------------------------------------------------*/
osSemaphoreId semRun;
osSemaphoreId semRxChar;
osSemaphoreId semUart;

uint8_t BufferToWrite[256];
int32_t BytesToWrite;
HAR_algoIdx_t HarAlgo = HAR_ALGO_IDX_NONE;

RTC_DateTypeDef CurrentDate;
RTC_TimeTypeDef CurrentTime;
#if SENSING1_USE_DATALOG
static volatile uint32_t SD_CardLogging  =0;
#endif /* SENSING1_USE_DATALOG */
#if SENSING1_USE_PRINTF
uint8_t Sensing1PrintfEnabled = 1;
#endif /* SENSING1_USE_PRINTF */
/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

uint8_t bdaddr[6];
uint8_t  NodeName[8];

/* Table with All the known Meta Data */
MDM_knownGMD_t known_MetaData[]={
  {GMD_NODE_NAME,      (sizeof(NodeName))},
  {GMD_END    ,0}/* THIS MUST BE THE LAST ONE */
};

uint16_t PedometerStepCount= 0;

/* Private variables ---------------------------------------------------------*/
static volatile int           ButtonPressed    = 0;

#ifndef USE_STM32L475E_IOT01
static volatile int           MEMSInterrupt    = 0;
#endif /* USE_STM32L475E_IOT01 */

#ifdef STM32_SENSORTILEBOX
volatile int PowerButtonPressed                = 0;
#endif /* STM32_SENSORTILEBOX */

volatile int RebootBoard                       = 0;

static volatile uint32_t      RunASCEvent      = 0;
static volatile uint32_t      SendEnv          = 0;
static volatile uint32_t      SendAudioLevel   = 0;
static volatile uint32_t      SendAccGyroMag   = 0;
static volatile uint32_t      ledTimer         = 0;
volatile uint32_t             MultiNN          = 0;

static volatile hostLinkType_t hostConnection  = NOT_CONNECTED ;

#if SENSING1_USE_BATTERY
static volatile uint32_t SendBatteryInfo       = 0;
#endif /* SENSING1_USE_BATTERY */

static volatile uint32_t UpdateMotionAR        = 0;

static uint32_t index_buff_fill = 0;
static int hciProcessEnable = 1;
static int audioInProgress = 0 ;

/* Private function prototypes -----------------------------------------------*/

static void Init_BlueNRG_Custom_Services(void);
static void Init_BlueNRG_Stack(void);

static unsigned char ReCallNodeNameFromMemory(void);

static void SendEnvironmentalData(void);
#ifndef USE_STM32L475E_IOT01
static void MEMSCallback(void);
#endif /* USE_STM32L475E_IOT01 */

static void ButtonCallback(void);
static void SendMotionData(void);
static void SendAudioLevelData(void);

static void LedBlinkCb(void const *arg);

static int  HardwareInit(void);


static void ProcessThread(void const *argument);
static void HostThread   (void const *argument);

#if SENSING1_USE_CLI
static void UARTConsoleThread(void const *argument);
#endif /* SENSING1_USE_CLI */

#if SENSING1_USE_BATTERY
static void SendBatteryInfoData(void);
#endif /* SENSING1_USE_BATTERY */

static void AudioProcess(void);
void AudioProcess_FromMics(void);
void AudioProcess_DB_Noise(void);

#ifdef SENSING1_BlueNRG2
  void APP_UserEvtRx(void *pData);
#endif /* SENSING1_BlueNRG2 */

static void ComputeMotionAR(void);
static void RunASC(void);
static void startProcessing (void const *arg);

static ASC_OutputTypeDef ascResultStored = ASC_UNDEFINED;
static HAR_output_t ActivityCodeStored = HAR_NOACTIVITY;
static MultiNN_Output_t MultiNN_OutStored = {HAR_NOACTIVITY,ASC_UNDEFINED};

#if SENSING1_USE_PRINTF
static void DisplayFirmwareInfo(void);
#else /* SENSING1_USE_PRINTF */
  #define DisplayFirmwareInfo(...)
#endif /* SENSING1_USE_PRINTF */

osTimerId timLedId,timEnvId,timMotionId;
osTimerId timAudioLevId,timActivityId;

/* CMSIS-OS  definitions */
/* threads */
#ifdef USE_STM32L475E_IOT01
osThreadDef(THREAD_1, ProcessThread, osPriorityNormal     , 0, configMINIMAL_STACK_SIZE*9);
osThreadDef(THREAD_2, HostThread   , osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE*6);
#else
osThreadDef(THREAD_1, ProcessThread, osPriorityNormal     , 0, configMINIMAL_STACK_SIZE*8);
osThreadDef(THREAD_2, HostThread   , osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE*5);
#endif
#if SENSING1_USE_CLI
  osThreadDef(THREAD_3, UARTConsoleThread, osPriorityLow    , 0, configMINIMAL_STACK_SIZE*3);
#endif /* SENSING1_USE_CLI  */ 
/* Semaphores */
osSemaphoreDef(SEM_Sm1);
osSemaphoreDef(SEM_Sm2);
osSemaphoreDef(SEM_Sm3);

/* Mail Queue */
osMailQId  mail;
#ifndef USE_STM32L4XX_NUCLEO
  osMailQDef(mail, 65, msgData_t);
#else /* USE_STM32L4XX_NUCLEO */
  osMailQDef(mail, 50, msgData_t);
#endif /* USE_STM32L4XX_NUCLEO */

/* Timers */
osTimerDef (TimerLedHandle , LedBlinkCb);
osTimerDef (TimerEnvHandle , startProcessing);
#if SENSING1_USE_BATTERY
  osTimerId timBatId;
  osTimerDef (TimerBatHandle , startProcessing);
#endif /* SENSING1_USE_BATTERY */
osTimerDef (TimerMotionHandle, startProcessing);
osTimerDef (TimerAudioLevHandle, startProcessing);
osTimerDef (TimerActivityHandle, startProcessing);

#if SENSING1_USE_DATALOG
  osTimerId timSdCardLoggingId;
  osTimerDef (TimerSdRecordingHandle , startProcessing);
#endif /* SENSING1_USE_DATALOG */

/* FreeRTOS CLI ------------------------------------------------------------- */
#if SENSING1_USE_CLI
static const char * const pcWelcomeMessage =
  "\r\nConsole command server.\r\nType 'help' to view a list of registered commands.\r\n";
static const char * const pcPromptMessage = "\r\n$ ";

#define MAX_INPUT_LENGTH    50
#define MAX_OUTPUT_LENGTH   200

/* The input and output buffers are declared static to keep them off the stack. */
static char pcOutputString[MAX_OUTPUT_LENGTH];
static char pcInputString[MAX_INPUT_LENGTH];

uint8_t cRxedChar;

#endif /* SENSING1_USE_CLI */

/* End of FreeRTOS CLI ------------------------------------------------------ */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  HardwareInit();

#if ( configUSE_TRACE_FACILITY == 1 )
  vTraceEnable(TRC_START);
#endif

  /* Create threads */
  osThreadCreate(osThread(THREAD_1), NULL);
  osThreadCreate(osThread(THREAD_2), NULL);
#if SENSING1_USE_CLI
  osThreadCreate(osThread(THREAD_3), NULL);
#endif /* SENSING1_USE_CLI */

  /* Register commands with the FreeRTOS+CLI command interpreter. */
  RegisterCLICommands();

  /* Create the semaphores */
  semRun = osSemaphoreCreate(osSemaphore(SEM_Sm1), 1);
  semRxChar = osSemaphoreCreate(osSemaphore(SEM_Sm2), 1);
  semUart = osSemaphoreCreate(osSemaphore(SEM_Sm3), 1);

  /* create mail queue */
  mail = osMailCreate(osMailQ(mail), NULL);

  /* set lowest reachable power mode  */
#if (defined(STM32_SENSORTILE) && SENSING1_USE_PRINTF)
  SetMinPowerMode(IDLE_WFI_TICK_SUPRESS);
#else
  SetMinPowerMode(IDLE_SLEEP_STOP);
#endif

  /* Start scheduler  */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler  */
  for (;;);
}

/**
  * @brief  Process Thread Function
  * @param  None
  * @retval None
  */
static void ProcessThread(void const *argument)
{
  msgData_t msg;

  while (1){
  if (semRun != NULL){
    if(osSemaphoreWait(semRun, osWaitForever) == osOK) {
      if(set_connectable){
        if(NecessityToSaveMetaDataManager) {
          uint32_t Success = EraseMetaDataManager();
          if(Success) {
            SaveMetaDataManager();
          }
        }
        msg.type  = SET_CONNECTABLE ;
        SendMsgToHost(&msg);
        set_connectable =0;
      }
#ifndef USE_STM32L475E_IOT01
      /* Handle Interrupt from MEMS */
      if(MEMSInterrupt) {
        MEMSCallback();
        MEMSInterrupt=0;
      }
#endif /* USE_STM32L475E_IOT01 */
      /* Handle user button */
      if(ButtonPressed) {
        ButtonCallback();
        ButtonPressed=0;
      }

#ifdef STM32_SENSORTILEBOX
      /* Power Off the SensorTile.box */
      if(PowerButtonPressed){
        BSP_BC_CmdSend(SHIPPING_MODE_ON);
        PowerButtonPressed =0;
      }
#endif /* STM32_SENSORTILEBOX */

      /* Environmental Data */
      if(SendEnv) {
        SendEnv=0;
        SendEnvironmentalData();
      }

      /* Mic Data */
      if (SendAudioLevel) {
        SendAudioLevel = 0;
        SendAudioLevelData();
      }

      /* Reboot the Board */
      if(RebootBoard) {
        RebootBoard=0;
        HAL_NVIC_SystemReset();
      }

      /* ASC */
      if (RunASCEvent) {
        RunASCEvent = 0;
        RunASC();
      }

      /* Motion Data */
      if(SendAccGyroMag) {
        SendAccGyroMag=0;
        SendMotionData();
      }

      if(UpdateMotionAR) {
        UpdateMotionAR=0;
        ComputeMotionAR();
      }

#if SENSING1_USE_DATALOG
      if(SD_CardLogging) {
        /* For MEMS data */
        SD_CardLogging=0;
        SdCardMemsRecordingRun(0);
      }

      /* For Audio data */
      if(writeAudio_flag) {
        writeAudio_flag=0;
        SdCardAudioRecordingRun();
      }
#endif /* SENSING1_USE_DATALOG */

#if SENSING1_USE_BATTERY
      /* Battery Info Data */
      if(SendBatteryInfo) {
        SendBatteryInfo=0;
        SendBatteryInfoData();
      }
#endif /* SENSING1_USE_BATTERY */
      }
    }
  }
}

/**
  * @brief  Function for sending Messages from Thread to Host Process
  * @param  None
  * @retval None
  */
int SendMsgToHost(msgData_t *msgPtr)
{
  msgData_t *ptr;
  if (mail) {
    /* Allocate memory */
    ptr = osMailAlloc(mail, osWaitForever);
    if (ptr) {
      BLUENRG_memcpy(ptr,msgPtr, sizeof(msgData_t));
      osMailPut(mail, ptr);
    } else {
      SENSING1_PRINTF("SendMsgToHost: mem allocation failed %d\r\n",msgPtr->type);
      return 0;
    }
  }
  return 1;
}

/**
  * @brief  Host Thread Function
  * @param  None
  * @retval None
  */
static void HostThread(void const *argument)
{
  msgData_t *msgPtr;
  osEvent  evt;

  for (;;) {
    /* wait for mail */
    evt = osMailGet(mail, osWaitForever);
    if (evt.status == osEventMail) {
      msgPtr = evt.value.p;
      switch(msgPtr->type) {
        case SET_CONNECTABLE:
          hciProcessEnable = 1 ;
          setConnectable();
          hostConnection = NOT_CONNECTED;
          LedBlinkStart();
          break;
        case SET_HOST_LINK_TYPE:
          if (msgPtr->HostLinkType != hostConnection) {
#ifdef BLE_LINK_ADAPT
            switch (msgPtr->HostLinkType) {
              case AUDIO_HOST_LINK:
                setConnectionParameters(8,17,0,400);
                break;
              case MOTION_HOST_LINK:
                setConnectionParameters(8,17,0,400);
                break;
              case ENV_HOST_LINK:
                setConnectionParameters(400,400,0,400);
                break;
              case DEFAULT_HOST_LINK:
                setConnectionParameters(400,400,0,400);
                break;
              default:
                break;
            }
#endif
            hostConnection = msgPtr->HostLinkType;
          }
          break;

        case  PROCESS_EVENT :
          if (hciProcessEnable) {
            hci_user_evt_proc();
          }
          break;

        case  CONF_NOTIFY :
          Config_NotifyBLE(msgPtr->conf.feature,msgPtr->conf.command,msgPtr->conf.data);
          break;
#ifndef USE_STM32L475E_IOT01
        case  ACC :
          AccEvent_Notify(msgPtr->acc, 2);
          break;
        case  ACC_STEP :
          if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) {
            AccEvent_Notify(msgPtr->stepCnt, 2);
          }
          if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) {
            AccEvent_Notify(msgPtr->stepCnt, 3);
          }
          break;
#endif /* USE_STM32L475E_IOT01 */

        case  AUDIO_LEV :
          AudioLevel_Update(msgPtr->DBNOISE_Value_Ch);
          break;
        case  ENV :
          Environmental_Update(msgPtr->env.press,msgPtr->env.hum,msgPtr->env.temp1,msgPtr->env.temp2);
          break;

        case MOTION :
          AccGyroMag_Update(&(msgPtr->motion.acc),&(msgPtr->motion.gyr),&(msgPtr->motion.mag));
          break;
        case ACTIVITY_GMP:
        case ACTIVITY_IGN:
        case ACTIVITY_IGN_WSDM:
          ActivityRec_Update(msgPtr->activity,HarAlgo);
          break;
        case AUDIO_SC:
          AudioSRec_Update(msgPtr->audio_scene);
          break;
        case MULTI_NN:
          break;

#if SENSING1_USE_BATTERY
        case BATTERY_INFO:
          GG_Update(msgPtr->batteryInfo.soc_status, msgPtr->batteryInfo.voltage,
                    msgPtr->batteryInfo.current);
          break;
#endif /* SENSING1_USE_BATTERY */

#if SENSING1_USE_DATALOG
      case SD_CARD_LOGGING:
        break;
#endif /* SENSING1_USE_DATALOG */
        case TERM_STDOUT:
          UpdateTermStdOut(msgPtr->term.data,msgPtr->term.length);
          break;

        case TERM_STDERR:
          UpdateTermStdErr(msgPtr->term.data,msgPtr->term.length);
          break;

        default :
          SENSING1_PRINTF("HostThread unexpected message:%d\r\n",msgPtr->type );
      }

      /* free memory allocated for mail */
      osMailFree(mail, msgPtr);

      /* check subsequent processing    */
      if(semRun) {
        osSemaphoreRelease(semRun);
      }
    }
  }
}

#if SENSING1_USE_CLI
/*
 * @brief    The task that implements the command interpreter using FreeRTOS+CLI.
 * @param    argument No used.
 *
 * @retval none.
 */
static void UARTConsoleThread(void const *argument)
{
    uint32_t cInputIndex = 0;
    BaseType_t xMoreDataToFollow;

    /* Enable UART interrupts */
#if defined(USE_STM32L4XX_NUCLEO)
    HAL_NVIC_SetPriority(USART2_IRQn, 0x0FU, 0x00);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
#elif defined(USE_STM32L475E_IOT01)
    HAL_NVIC_SetPriority(USART1_IRQn, 0x0FU, 0x00);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif /* USE_STM32L4XX_NUCLEO */

    /* Send a welcome message to the user knows they are connected. */
    _SENSING1_PRINTF(pcWelcomeMessage);
    _SENSING1_PRINTF(pcPromptMessage);
    SENSING1_PRINTF_FLUSH();

    // TODO: Disable UART Rx when low-power mode is requested (e.g. button press)

    // cRxedChar = getchar();

#if SENSING1_USE_USB_CDC
    /* USB CDC Rx is always on when USB CDC is active */
#else
    // Take semaphore
    if (osSemaphoreWait(semRxChar, osWaitForever) != osOK)
    {
        while(1);
    }

    if (osSemaphoreWait(semUart, osWaitForever) != osOK)
    {
        while(1);
    }
    if (HAL_UART_Receive_IT(&UartHandle, &cRxedChar, 1) != HAL_OK)
    {
        while(1);
    }
    osSemaphoreRelease(semUart);
#endif /* SENSING1_USE_USB_CDC */

    for (;;)
    {
        if (osSemaphoreWait(semRxChar, osWaitForever) == osOK)
        {
            /* echo back character */
            if (cRxedChar == ESC)
            {
              if (Sensing1PrintfEnabled)
              {
                SENSING1_PRINTF("\n\r<esc> key is pressed");
                SENSING1_PRINTF("\n\rInhibiting Console Out mode is entered");
                SENSING1_PRINTF("\n\rmode will auto-exit after next command");
                SENSING1_PRINTF("\n\ror hit <esc> key again to exit manually\n\r");
                Sensing1PrintfEnabled = !Sensing1PrintfEnabled;
              }
              else
              {
                Sensing1PrintfEnabled = !Sensing1PrintfEnabled;
                SENSING1_PRINTF("\n\r<esc> key is pressed");
                SENSING1_PRINTF("\n\rInhibiting Console Out mode is exited");
                SENSING1_PRINTF("\n\rhit <esc> key to enter this mode again\n\r");
              }
              _SENSING1_PRINTF(pcPromptMessage);
            }
            else
            {
              _SENSING1_PRINTF("%c", cRxedChar);
            }
            SENSING1_PRINTF_FLUSH();

            // cRxedChar = getchar();
            /* This implementation reads a single character at a time. */
            /* Note: A small delay is required between each character transmission.
                     In TeraTerm, Setup -> Serial Port...: Set Transmit delay to 1 msec/char */
    #if SENSING1_USE_USB_CDC
    #else
            if (osSemaphoreWait(semUart, osWaitForever) != osOK)
            {
                while(1);
            }
            if (HAL_UART_Receive_IT(&UartHandle, &cRxedChar, 1) != HAL_OK)
            {
                while(1);
            }
            osSemaphoreRelease(semUart);
    #endif /* SENSING1_USE_USB_CDC */

            // if( cRxedChar == '\n' || cRxedChar == '\r' )
            if (cRxedChar == '\r' )
            // if( cRxedChar == '\n' )
            {
                /* Send LF to avoid issues when only CR has been sent */
                SENSING1_PRINTF("\n");

                if (!Sensing1PrintfEnabled)
                {
                  Sensing1PrintfEnabled = !Sensing1PrintfEnabled;
                  SENSING1_PRINTF("\n\r<esc> key is pressed");
                  SENSING1_PRINTF("\n\rInhibiting Console Out mode is auto-exited");
                  _SENSING1_PRINTF(pcPromptMessage);
                }
                if (strlen(pcInputString) != 0)
                {
                    do {
                        /* Send the command string to the command interpreter.  Any
                        output generated by the command interpreter will be placed in the
                        pcOutputString buffer. */
                        xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
                            pcInputString,    /* The command string.*/
                            pcOutputString,   /* The output buffer. */
                            MAX_OUTPUT_LENGTH /* The size of the output buffer. */
                        );

                        /* Write the output generated by the command interpreter to the
                        console. */
                        _SENSING1_PRINTF(pcOutputString);
                    } while (xMoreDataToFollow != 0);
                }

                /* All the strings generated by the input command have been sent.
                Processing of the command is complete.  Clear the input string ready
                to receive the next command. */
                cInputIndex = 0;
                memset(pcInputString, 0x00, MAX_INPUT_LENGTH);

                /* Prompt */
                _SENSING1_PRINTF(pcPromptMessage);
                SENSING1_PRINTF_FLUSH();
            }
            else
            {
                /* The if() clause performs the processing after a newline character
                is received.  This else clause performs the processing if any other
                character is received. */

                // if (cRxedChar == '\r' )
                // {
                //     /* Ignore carriage returns. */
                // }
                // else
                if (cRxedChar == '\b')
                {
                    /* Backspace was pressed.  Erase the last character in the input
                    buffer - if there are any. */
                    if (cInputIndex > 0 )
                    {
                        cInputIndex--;
                        pcInputString[ cInputIndex ] = '\0';
                        /* Hack to erase characters */
                        _SENSING1_PRINTF(" \b");
                    }
                }
                else if ((cRxedChar != ESC) && (cRxedChar != '\0'))
                {
                    /* A character was entered.  It was not a new line, backspace
                    or carriage return, so it is accepted as part of the input and
                    placed into the input buffer.  When a \n is entered the complete
                    string will be passed to the command interpreter. */
                    if (cInputIndex < MAX_INPUT_LENGTH)
                    {
                        pcInputString[ cInputIndex ] = cRxedChar;
                        cInputIndex++;
                    }
                }
            }
        }
    }
}
#endif /* SENSING1_USE_CLI */

/**
  * @brief  System Initialization function
  * @param  None
  * @retval None
  */
static int HardwareInit(void)
{
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();

  initPowerController();

#ifdef USE_STM32L4XX_NUCLEO
  InitTargetPlatform(TARGET_NUCLEO);
#elif defined(STM32_SENSORTILE)
  InitTargetPlatform(TARGET_SENSORTILE);
#elif defined(USE_STM32L475E_IOT01)
  InitTargetPlatform(TARGET_IOT01A1);
#elif defined(STM32_SENSORTILEBOX)
  InitTargetPlatform(TARGET_SENSORTILEBOX);
#else
  #error "Define the right platform"
#endif /* USE_STM32L4XX_NUCLEO */

  DisplayFirmwareInfo();

  /* Check the MetaDataManager */
 InitMetaDataManager((void *)&known_MetaData,MDM_DATA_TYPE_GMD,NULL);

  /* Set Node Name */
  ReCallNodeNameFromMemory();

  /* Initialize the BlueNRG */
  Init_BlueNRG_Stack();

  /* Initialize the BlueNRG Custom services */
  Init_BlueNRG_Custom_Services();

  /* Check the BootLoader Compliance */
  if(CheckBootLoaderCompliance()) {
    SENSING1_PRINTF("BootLoader Compliant with FOTA\r\n");
  } else {
    SENSING1_PRINTF("ERROR: BootLoader NOT Compliant with FOTA\r\n");
  }
  SENSING1_PRINTF_FLUSH();

#ifdef STM32_SENSORTILEBOX
  PowerButtonPressed  =0;
#endif /* STM32_SENSORTILEBOX */
  return 0;
}

static void startProcessing  (void const *arg)
{
  if(arg == timEnvId){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_ENV)) {
      SendEnv=1;
    }
  }
#if SENSING1_USE_BATTERY
  else if     (arg == timBatId) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_GG_EVENT)) {
      SendBatteryInfo= 1;
    }
  }
#endif /* SENSING1_USE_BATTERY */

  else if (arg == timMotionId){
    SendAccGyroMag=1;
  }
  else if (arg == timAudioLevId){
    SendAudioLevel=1;
  }
  else if (arg == timActivityId){
    UpdateMotionAR=1;
  }
#if SENSING1_USE_DATALOG
  else if(arg == timSdCardLoggingId){
    SD_CardLogging= 1;
  }
#endif /* SENSING1_USE_DATALOG */
  else{
    SENSING1_PRINTF("wrong timer : %ld\n",(uint32_t)arg);
  }
  if(semRun) {
    osSemaphoreRelease(semRun);
  }
}

int startProc(msgType_t type,uint32_t period)
{
  msgData_t msg,msgAcq;
  msg.type         = SET_HOST_LINK_TYPE;
  msg.HostLinkType = DEFAULT_HOST_LINK;
  osTimerId id     = NULL;
  switch (type)
  {
#if SENSING1_USE_BATTERY
    case BATTERY_INFO:
      if (!timBatId) {
        timBatId = osTimerCreate (osTimer(TimerBatHandle), osTimerPeriodic, NULL);
      }
      id = timBatId;
      break;
#endif /* SENSING1_USE_BATTERY */

    case ENV:
      if (!timEnvId) {
        timEnvId = osTimerCreate (osTimer(TimerEnvHandle), osTimerPeriodic, NULL);
      }
      if(!TargetBoardFeatures.EnvSensorEnabled) {
        EnableEnvSensors();
      }
      msg.HostLinkType  = ENV_HOST_LINK;
      id = timEnvId;
       break;
    case MOTION:
      if (!timMotionId) {
        timMotionId = osTimerCreate (osTimer(TimerMotionHandle),osTimerPeriodic, NULL);
      }
      EnableMotionSensors ();
      Set2GAccelerometerFullScale();
      msg.HostLinkType  = MOTION_HOST_LINK;
      id = timMotionId;
      break;
    case AUDIO_LEV:
      if (!timAudioLevId) {
        timAudioLevId = osTimerCreate (osTimer(TimerAudioLevHandle),osTimerPeriodic, NULL);
      }
      id = timAudioLevId;
      PowerCtrlLock();
      msg.HostLinkType  = AUDIO_HOST_LINK;
      audioInProgress |= 0x1 ;
      break;
    case MULTI_NN:
      /* Initialize Acoustic Scene Recognition */
      ActivityCodeStored        = HAR_NOACTIVITY;
      ascResultStored           = ASC_UNDEFINED;
      MultiNN_OutStored.harOut  = ActivityCodeStored;
      MultiNN_OutStored.ascOut  = ascResultStored;

      ASC_Init();
      if (HarAlgo == HAR_ALGO_IDX_NONE )
          HarAlgo = HAR_IGN_IDX;
      if (!timActivityId) {
        timActivityId = osTimerCreate (osTimer(TimerActivityHandle),osTimerPeriodic, NULL);
      }
      MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      Set4GAccelerometerFullScale();
      MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,INERTIAL_ACQ_ACTIVITY_IGN_HZ);
      HAR_Initialize(HarAlgo);
      id = timActivityId;
      msgAcq.type        = AUDIO_SC;
      msgAcq.audio_scene = ascResultStored;
      SendMsgToHost(&msgAcq);
      msgAcq.type        = ACTIVITY_IGN;
      msgAcq.activity    = ActivityCodeStored;
      SendMsgToHost(&msgAcq);

      break;

    case AUDIO_SC:
      /* Initialize Acoustic Scene Recognition */
      ascResultStored = ASC_UNDEFINED;
      PowerCtrlLock();
    #if SENSING1_USE_USB_AUDIO
      InitUSBAudio();
    #else
      InitMics(AUDIO_SAMPLING_FREQUENCY);
    #endif /* SENSING1_USE_USB_AUDIO */
      ASC_Init();
      msgAcq.type        = AUDIO_SC;
      msgAcq.audio_scene = ascResultStored;
      SendMsgToHost(&msgAcq);
      break;

    case ACTIVITY_GMP:
     if (HarAlgo == HAR_ALGO_IDX_NONE )
          HarAlgo = HAR_GMP_IDX;
      if (!timActivityId) {
        timActivityId = osTimerCreate (osTimer(TimerActivityHandle),osTimerPeriodic, NULL);
      }
      MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      Set4GAccelerometerFullScale();
      MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,INERTIAL_ACQ_ACTIVITY_GMP_HZ);
      HAR_Initialize(HarAlgo);
      ActivityCodeStored = HAR_NOACTIVITY;
      msgAcq.type        = ACTIVITY_GMP;
      msgAcq.activity    = ActivityCodeStored;
      SendMsgToHost(&msgAcq);
      id = timActivityId;
      break;

  case ACTIVITY_IGN:
      if (HarAlgo == HAR_ALGO_IDX_NONE )
          HarAlgo = HAR_IGN_IDX;
      if (!timActivityId) {
        timActivityId = osTimerCreate (osTimer(TimerActivityHandle),osTimerPeriodic, NULL);
      }
      MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      Set4GAccelerometerFullScale();
      MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,INERTIAL_ACQ_ACTIVITY_IGN_HZ);
      HAR_Initialize(HarAlgo);
      ActivityCodeStored = HAR_NOACTIVITY;
      msgAcq.type        = ACTIVITY_IGN;
      msgAcq.activity    = ActivityCodeStored;
      SendMsgToHost(&msgAcq);
      id = timActivityId;
      break;

  case ACTIVITY_IGN_WSDM:
      if (HarAlgo == HAR_ALGO_IDX_NONE )
          HarAlgo = HAR_IGN_WSDM_IDX;
      if (!timActivityId) {
        timActivityId = osTimerCreate (osTimer(TimerActivityHandle),osTimerPeriodic, NULL);
      }
      MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      Set2GAccelerometerFullScale();
      MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,INERTIAL_ACQ_ACTIVITY_IGN_WSDM_HZ);
      HAR_Initialize(HarAlgo);
      ActivityCodeStored = HAR_NOACTIVITY;
      msgAcq.type        = ACTIVITY_IGN_WSDM;
      msgAcq.activity    = ActivityCodeStored;
      SendMsgToHost(&msgAcq);
      id = timActivityId;
      break;

#if SENSING1_USE_DATALOG
     case SD_CARD_LOGGING:
       if (!timSdCardLoggingId) {
         timSdCardLoggingId = osTimerCreate (osTimer(TimerSdRecordingHandle), osTimerPeriodic, NULL);
       }
       id = timSdCardLoggingId;
       break;
#endif /* SENSING1_USE_DATALOG */
    default :
      SENSING1_PRINTF("wrong type : %d\n",type);
      break;
  }
  if (id){
    if  (osTimerStart (id, period) != osOK){
      SENSING1_PRINTF("failed starting timer\n");
    }

  }
  SendMsgToHost(&msg);
  return 0;
}

int stopProc(msgType_t type)
{
  msgData_t msg;
  msg.type          = SET_HOST_LINK_TYPE;
  msg.HostLinkType  = DEFAULT_HOST_LINK;
  osTimerId id = NULL;
  switch (type) {
#if SENSING1_USE_BATTERY
    case BATTERY_INFO:
      id           = timBatId;
      timBatId     = NULL;
      break;
#endif /* SENSING1_USE_BATTERY */
    case ENV:
      id           = timEnvId;
      timEnvId     = NULL;
      DisableEnvSensors();
      break;
    case MOTION:
      id            = timMotionId;
      timMotionId   = NULL;
      DisableMotionSensors ();
      break;
    case AUDIO_LEV:
      id            = timAudioLevId;
      if (audioInProgress&0x1) {
        PowerCtrlUnLock();
      }
      timAudioLevId = NULL;
      audioInProgress &= 0x2+0x4 ;
      break;
    case MULTI_NN:
      /* DeInitialize Acoustic Scene Recognition */
      ASC_DeInit();
      id            = timActivityId;
      timActivityId = NULL;
      MOTION_SENSOR_Disable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      HAR_DeInitialize(HarAlgo);
      HarAlgo = HAR_ALGO_IDX_NONE;
      break;

    case AUDIO_SC:
      /* DeInitialize Acoustic Scene Recognition */
      ASC_DeInit();
    #if SENSING1_USE_USB_AUDIO
      DeInitUSBAudio();
    #else
      DeInitMics();
    #endif /* SENSING1_USE_USB_AUDIO */
      PowerCtrlUnLock();
      break;
    case ACTIVITY_GMP:
    case ACTIVITY_IGN:
    case ACTIVITY_IGN_WSDM:
      id            = timActivityId;
      timActivityId = NULL;
      MOTION_SENSOR_Disable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
      HAR_DeInitialize(HarAlgo);
      HarAlgo = HAR_ALGO_IDX_NONE;
      break;

#if SENSING1_USE_DATALOG
    case SD_CARD_LOGGING:
      id                 = timSdCardLoggingId;
      timSdCardLoggingId = NULL;
      break;
#endif /* SENSING1_USE_DATALOG */

  default :
      break;
  }
  if (id){
    if  (osTimerStop (id) != osOK){
      SENSING1_PRINTF("could not stop timer\n");
    }
    if (osTimerDelete (id) != osOK)  {
      SENSING1_PRINTF("could not delete timer\n");
    }
  }
  SendMsgToHost(&msg);
  return 0;
}

/**
  * @brief  This function sets the ACC FS to 2g
  * @param  None
  * @retval None
  */
void Set2GAccelerometerFullScale(void)
{
  float sensitivity;
  /* Set Full Scale to +/-2g */
  MOTION_SENSOR_SetFullScale( TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,2);

  /* Read the Acc Sensitivity */
  MOTION_SENSOR_GetSensitivity(TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,&sensitivity);
  TargetBoardFeatures.AccSensiMultInG = sensitivity * FROM_MG_TO_G ;
}

/**
  * @brief  This function dsets the ACC FS to 4g
  * @param  None
  * @retval None
  */
void Set4GAccelerometerFullScale(void)
{
  float sensitivity;

  /* Set Full Scale to +/-4g */
  MOTION_SENSOR_SetFullScale( TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,4);

  /* Read the Acc Sensitivity */
  MOTION_SENSOR_GetSensitivity(TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,&sensitivity);
  TargetBoardFeatures.AccSensiMultInG = sensitivity * FROM_MG_TO_G;
}

#if SENSING1_USE_PRINTF
/**
  * @brief Display all the Firmware Information
  * @param  None
  * @retval None
  */
static void DisplayFirmwareInfo(void)
{
    SENSING1_PRINTF("\r\n------------------------------------------------------------\r\n");
    SENSING1_PRINTF("STMicroelectronics %s\r\n"
         "\tVersion %c.%c.%c\r\n"
#ifdef STM32_SENSORTILE
        "\tSTM32476RG-SensorTile board"
#elif defined(USE_STM32L4XX_NUCLEO)
        "\tSTM32L476RG-Nucleo board"
#elif defined(USE_STM32L475E_IOT01)
        "\tSTM32L475R-IoT01A1 board"
#elif defined(STM32_SENSORTILEBOX)
        "\tSTM32L4R9ZI-SensorTile.box board"
#else
  #error "Define something here"
#endif /* STM32_SENSORTILE */
          "\r\n\n",
          SENSING1_PACKAGENAME,
          SENSING1_VERSION_MAJOR,SENSING1_VERSION_MINOR,SENSING1_VERSION_PATCH);

    SENSING1_PRINTF("\t(HAL %ld.%ld.%ld_%ld)\r\n\tCompiled %s %s"
#if defined (__IAR_SYSTEMS_ICC__)
        " (IAR)\r\n",
#elif defined (__CC_ARM)
        " (KEIL)\r\n",
#elif defined (__GNUC__)
        " (openstm32)\r\n",
#endif
           HAL_GetHalVersion() >>24,
          (HAL_GetHalVersion() >>16)&0xFF,
          (HAL_GetHalVersion() >> 8)&0xFF,
           HAL_GetHalVersion()      &0xFF,
           __DATE__,__TIME__);

  SENSING1_PRINTF("------------------------------------------------------------\r\n");
}
#endif /* SENSING1_USE_PRINTF */

/**
  * @brief  Callback for user button
  * @param  None
  * @retval None
  */
static void ButtonCallback(void)
{
  SENSING1_PRINTF("UserButton Pressed\r\n");
}

#ifndef USE_STM32L475E_IOT01
void  AccEvent_Msg(AccEventType Event)
{
  msgData_t msg;
  msg.type  = ACC;
  msg.acc   = Event;
  SendMsgToHost(&msg);
}
void  AccStepEvent_Msg(uint16_t stepCnt)
{
  msgData_t msg;
  msg.type    = ACC_STEP;
  msg.stepCnt = stepCnt;
  SendMsgToHost(&msg);
}

/**
  * @brief  Send Notification where there is a interrupt from MEMS
  * @param  None
  * @retval None
  */
static void MEMSCallback(void)
{
  MOTION_SENSOR_Event_Status_t status;

  MOTION_SENSOR_Get_Event_Status(TargetBoardFeatures.HandleAccSensor,&status);

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to Pedometer */
    if(status.StepStatus != 0) {
      PedometerStepCount = GetStepHWPedometer();
      if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) {
        AccStepEvent_Msg(PedometerStepCount);
      }
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_FREE_FALL)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to Free Fall */
    if(status.FreeFallStatus != 0) {
      AccEvent_Msg(ACC_FREE_FALL);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_SINGLE_TAP)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to Single Tap */
    if(status.TapStatus != 0) {
      AccEvent_Msg(ACC_SINGLE_TAP);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_DOUBLE_TAP)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to Double Tap */
    if(status.DoubleTapStatus != 0) {
      AccEvent_Msg(ACC_DOUBLE_TAP);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_TILT)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to Tilt */
    if(status.TiltStatus != 0) {
      AccEvent_Msg(ACC_TILT);
    }
  }

  if( (W2ST_CHECK_HW_FEATURE(W2ST_HWF_6DORIENTATION)) ||
      (W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) ) {
    /* Check if the interrupt is due to 6D Orientation */
    if(status.D6DOrientationStatus != 0) {
      AccEventType Orientation = GetHWOrientation6D();
      AccEvent_Msg(Orientation);
    }
  }

  if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_WAKE_UP)) {
    /* Check if the interrupt is due to Wake Up */
    if(status.WakeUpStatus != 0) {
      AccEvent_Msg(ACC_WAKE_UP);
    }
  }

  if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_MULTIPLE_EVENTS)) {
    AccStepEvent_Msg(PedometerStepCount);
  }
}
#endif /* USE_STM32L475E_IOT01 */

/**
  * @brief  Send Motion Data Acc/Mag/Gyro to BLE
  * @param  None
  * @retval None
  */
static void SendMotionData(void)
{
  MOTION_SENSOR_Axes_t ACC_Value;
  MOTION_SENSOR_Axes_t GYR_Value;
  MOTION_SENSOR_Axes_t MAG_Value;
  msgData_t msg;

  /* Read the Acc values */
  if(TargetBoardFeatures.HandleAccSensor != SENSING1_SNS_NOT_VALID ) {
    MOTION_SENSOR_GetAxes(TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,&ACC_Value);
  } else {
    ACC_Value.x = ACC_Value.y = ACC_Value.z =0;
  }

  /* Read the Magneto values */
  if(TargetBoardFeatures.HandleMagSensor != SENSING1_SNS_NOT_VALID ) {
    MOTION_SENSOR_GetAxes(TargetBoardFeatures.HandleMagSensor,MOTION_MAGNETO,&MAG_Value);
  } else {
    MAG_Value.x = MAG_Value.y = MAG_Value.z =0;
  }

  /* Read the Gyro values */
  if(TargetBoardFeatures.HandleGyroSensor != SENSING1_SNS_NOT_VALID ) {
    MOTION_SENSOR_GetAxes(TargetBoardFeatures.HandleGyroSensor,MOTION_GYRO,&GYR_Value);
  } else {
    GYR_Value.x = GYR_Value.y = GYR_Value.z =0;
  }

  msg.type        = MOTION;
  msg.motion.acc  = ACC_Value ;
  msg.motion.gyr  = GYR_Value;
  msg.motion.mag  = MAG_Value;
  SendMsgToHost(&msg);
}
static void SyncMultiNN(msgType_t msgType, uint32_t code)
{
  msgData_t msg;
  msg.type = MULTI_NN;

  if (ACTIVITY_IGN == msgType){
    MultiNN_OutStored.harOut = (HAR_output_t)code;
  }
  else if (AUDIO_SC == msgType){
    MultiNN_OutStored.ascOut = (ASC_OutputTypeDef)code;
  }
  SENSING1_PRINTF("<multi %d %d> \r\n",MultiNN_OutStored.harOut,MultiNN_OutStored.ascOut);
  msg.multiNN = MultiNN_OutStored ;
  SendMsgToHost(&msg);
}

/**
  * @brief  MotionAR Working function
  * @param  None
  * @retval None
  */
static void ComputeMotionAR(void)
{
  HAR_output_t ActivityCode;
  MOTION_SENSOR_AxesRaw_t ACC_Value_Raw;
  msgData_t msg;

  if(HarAlgo != HAR_ALGO_IDX_NONE)
  {
    /* Read the Acc RAW values */
    MOTION_SENSOR_GetAxesRaw(TargetBoardFeatures.HandleAccSensor,MOTION_ACCELERO,&ACC_Value_Raw);
    ActivityCode =  HAR_run(ACC_Value_Raw,HarAlgo);
    if(ActivityCodeStored!=ActivityCode){
      ActivityCodeStored = ActivityCode;
      if (MultiNN)
      {
        SyncMultiNN(ACTIVITY_IGN,(uint32_t)ActivityCode);
      }
      else
      {
         switch (HarAlgo){
          case HAR_GMP_IDX      : msg.type = ACTIVITY_GMP      ; break;
          case HAR_IGN_IDX      : msg.type = ACTIVITY_IGN      ; break;
          case HAR_IGN_WSDM_IDX : msg.type = ACTIVITY_IGN_WSDM ; break;
          default: break;
        }
        msg.activity       = ActivityCode ;
        SendMsgToHost(&msg);

        if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
           BytesToWrite = sprintf((char *)BufferToWrite,"Sending: AR=%d\n",ActivityCode);
           Term_Update(BufferToWrite,BytesToWrite);
        } else {
          SENSING1_PRINTF("Sending: AR=%d\r\n",ActivityCode);
        }
      }
    }
  }
}

/**
* @brief  Callback function when 1ms PCM Audio is received from Microphones
* @param  none
* @retval None
*/
void AudioProcess_FromMics(void)
{
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_ASC_EVENT)) {
    /* Copy PCM Buffer from Microphones onto FILL BUFFER */
    memcpy(Fill_Buffer + index_buff_fill, PCM_Buffer, sizeof(int16_t) * 16);
    index_buff_fill += 16;

    AudioProcess();
 }

#if SENSING1_USE_DATALOG
  if(SD_LogAudio_Enabled) {
    AudioProcess_SD_Recording(PCM_Buffer, PCM_AUDIO_IN_SAMPLES);
  } else
#endif /* SENSING1_USE_DATALOG */
  {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL)) {
      AudioProcess_DB_Noise();
    }
  }
}

/**
* @brief  Callback function when 1ms PCM Audio is received from USB
* @param  none
* @retval None
*/
void AudioProcess_FromPC(uint8_t *pData, uint32_t count)
{
  /* Copy PCM Buffer on FILL BUFFER */
  /* In this USB Audio configuration, the signal comes in as 2ch, extract 1ch */
  for (uint32_t i = 0; i < count / 4; i++) {
    Fill_Buffer[index_buff_fill] = ((int16_t *) pData)[2 * i];
    index_buff_fill++;
  }
  /* Use memcpy if input buffer is 1ch */
  // memcpy(Fill_Buffer + index_buff_fill, pData, sizeof(int16_t) * 16);
  // index_buff_fill += 16;

  AudioProcess();
}

/**
* @brief  Function that is called when data has been appended to Fill Buffer
* @param  none
* @retval None
*/
static void AudioProcess(void)
{
  float32_t sample;

  /* Create a 64ms (1024 samples) window every 32ms (512 samples)
   Audio Feature Extraction is ran every 32ms on a 64ms window (50% overlap) */
  if (index_buff_fill == FILL_BUFFER_SIZE) {
    /* Copy Fill Buffer in Proc Buffer */
    for (uint32_t i = 0; i < FILL_BUFFER_SIZE; i++) {
      sample = ((float32_t) Fill_Buffer[i]);
      /* Invert the scale of the data */
      sample /= (float32_t) ((1 << (8 * sizeof(int16_t) - 1)));
      Proc_Buffer_f[i] = sample;
    }

    /* Left shift Fill Buffer by 512 samples */
    memmove(Fill_Buffer, Fill_Buffer + (FILL_BUFFER_SIZE / 2), sizeof(int16_t) * (FILL_BUFFER_SIZE / 2));
    index_buff_fill = (FILL_BUFFER_SIZE / 2);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_ASC_EVENT)) {
      /* Release processing thread to start Audio Feature Extraction */
      RunASCEvent = 1;
    }

    if (semRun) {
      osSemaphoreRelease(semRun);
    }
  }
}

/**
* @brief  User function that is called when 1 ms of PDM data is available.
* @param  none
* @retval None
*/
void AudioProcess_DB_Noise(void)
{
  int32_t i;
  int32_t NumberMic;
  for(i = 0; i < 16; i++){
    for(NumberMic=0;NumberMic<AUDIO_CHANNELS;NumberMic++) {
      RMS_Ch[NumberMic] += (float)((int16_t)PCM_Buffer[i*AUDIO_CHANNELS+NumberMic] * ((int16_t)PCM_Buffer[i*AUDIO_CHANNELS+NumberMic]));
    }
  }
}

/**
  * @brief  Acoustic Scene Recognition Working function
  * @param  None
  * @retval None
  */
void RunASC(void)
{
  ASC_OutputTypeDef classification_result;
  msgData_t msg;

  /* ASC_Run needs to be called 32 times before it can run the NN and return a classification */
  classification_result = ASC_Run(Proc_Buffer_f);

  /* Only display classification result if a valid classification is returned */
  if (classification_result != ASC_UNDEFINED) {
    if (ascResultStored != classification_result) {
      ascResultStored = classification_result;
      if (MultiNN)
      {
        SyncMultiNN(AUDIO_SC,(uint32_t)classification_result);
      }
      else
      {
        msg.type       = AUDIO_SC;
        msg.audio_scene = classification_result;
        SendMsgToHost(&msg);

        if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
           BytesToWrite = sprintf((char *)BufferToWrite,"Sending: ASC=%d\n", classification_result);
           Term_Update(BufferToWrite,BytesToWrite);
        } else {
          SENSING1_PRINTF("Sending: ASC=%d\r\n", classification_result);
        }
      }
    }
  }
}

/**
  * @brief  Send Audio Level Data (Ch1) to BLE
  * @param  None
  * @retval None
  */
static void SendAudioLevelData(void)
{
  int32_t NumberMic;
  uint16_t DBNOISE_Value_Ch[AUDIO_CHANNELS];
  msgData_t msg;

  for(NumberMic=0;NumberMic<(AUDIO_CHANNELS);NumberMic++) {
    DBNOISE_Value_Ch[NumberMic] = 0;

    RMS_Ch[NumberMic] /= (16.0f*MICS_DB_UPDATE_MS);

    DBNOISE_Value_Ch[NumberMic] = (uint16_t)((120.0f - 20 * log10f(32768 * (1 + 0.25f * (TargetBoardFeatures.AudioVolume /*AudioInVolume*/ - 4))) + 10.0f * log10f(RMS_Ch[NumberMic])) * 0.3f + DBNOISE_Value_Old_Ch[NumberMic] * 0.7f);
    DBNOISE_Value_Old_Ch[NumberMic] = DBNOISE_Value_Ch[NumberMic];
    RMS_Ch[NumberMic] = 0.0f;
  }
  msg.type  = AUDIO_LEV;
  memcpy(&(msg.DBNOISE_Value_Ch),&DBNOISE_Value_Ch,AUDIO_CHANNELS* sizeof(uint16_t));
  SendMsgToHost(&msg);
}

#ifndef USE_STM32L475E_IOT01
/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  AudioProcess_FromMics();
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  AudioProcess_FromMics();
}

#else

/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
  uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

  /* Copy first half of PCM_Buffer from Microphones onto Fill_Buffer */
  memcpy(Fill_Buffer + index_buff_fill, PCM_Buffer, buffer_size);
  index_buff_fill += nb_samples;

  AudioProcess();

#if SENSING1_USE_DATALOG
  if (SD_LogAudio_Enabled)
  {
    AudioProcess_SD_Recording(PCM_Buffer, nb_samples);
  }
  else
#endif /* SENSING1_USE_DATALOG */
  {
    if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL))
    {
      AudioProcess_DB_Noise();
    }
  }
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
  uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

  /* Copy second half of PCM_Buffer from Microphones onto Fill_Buffer */
  memcpy(Fill_Buffer + index_buff_fill, PCM_Buffer + nb_samples, buffer_size);
  index_buff_fill += nb_samples;

  AudioProcess();

#if SENSING1_USE_DATALOG
  if (SD_LogAudio_Enabled)
  {
    AudioProcess_SD_Recording(PCM_Buffer + nb_samples, nb_samples);
  }
  else
#endif /* SENSING1_USE_DATALOG */
  {
    if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL))
    {
      AudioProcess_DB_Noise();
    }
  }
}
#endif /* USE_STM32L475E_IOT01 */

/**
  * @brief  Manages the BSP audio in error event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  Error_Handler();
}


/**
  * @brief  Read The Environmetal Data (Temperature/Pressure/Humidity)
  * @param  int32_t *PressToSend pointer to Press Value
  * @param  uint16_t *HumToSend  pointer to Humidity Value
  * @param  int16_t *Temp1ToSend pointer to Temperature1 Value
  * @param  int16_t *Temp2ToSend pointer to Temperature2 Value
  * @retval None
  */
void ReadEnvironmentalData(int32_t *PressToSend,uint16_t *HumToSend,int16_t *Temp1ToSend,int16_t *Temp2ToSend)
{
  float SensorValue;
  int32_t decPart, intPart;

  *PressToSend=0;
  *HumToSend=0;
  *Temp2ToSend=0,*Temp1ToSend=0;

  if(TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    /*Read the previous value of the sensor and restart the One Shot for the next measurement*/
    ENV_SENSOR_GetValue(TargetBoardFeatures.HandleHumSensor,ENV_HUMIDITY,&SensorValue);
    MCR_BLUEMS_F2I_1D(SensorValue, intPart, decPart);
    *HumToSend = intPart*10+decPart;

    if(TargetBoardFeatures.HandleTempSensors[0] != SENSING1_SNS_NOT_VALID) {
      ENV_SENSOR_GetValue(TargetBoardFeatures.HandleTempSensors[0],ENV_TEMPERATURE,&SensorValue);
      MCR_BLUEMS_F2I_1D(SensorValue, intPart, decPart);
      *Temp1ToSend = intPart*10+decPart;
    }
#ifdef ONE_SHOT
    ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandleHumSensor);
#endif
  }

  if(TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    /*Read the previous value of the sensor and restart the One Shot for the next measurement*/
    ENV_SENSOR_GetValue(TargetBoardFeatures.HandlePressSensor,ENV_PRESSURE,&SensorValue);
    MCR_BLUEMS_F2I_2D(SensorValue, intPart, decPart);
    *PressToSend=intPart*100+decPart;

    if(TargetBoardFeatures.HandleTempSensors[1] != SENSING1_SNS_NOT_VALID) {
      ENV_SENSOR_GetValue(TargetBoardFeatures.HandleTempSensors[1],ENV_TEMPERATURE,&SensorValue);
      MCR_BLUEMS_F2I_1D(SensorValue, intPart, decPart);
      *Temp2ToSend = intPart*10+decPart;
    }
#ifdef ONE_SHOT
    ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandlePressSensor);
#endif
  }
}

/**
  * @brief  Send Environmetal Data (Temperature/Pressure/Humidity) to BLE
  * @param  None
  * @retval None
  */
static void SendEnvironmentalData(void)
{
  msgData_t msg;

  /* Pressure,Humidity, and Temperatures*/
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_ENV)) {
    int32_t PressToSend;
    uint16_t HumToSend;
    int16_t Temp2ToSend,Temp1ToSend;

    /* Read all the Environmental Sensors */
    ReadEnvironmentalData(&PressToSend,&HumToSend, &Temp1ToSend,&Temp2ToSend);

    msg.type      = ENV;
    msg.env.press = PressToSend;
    msg.env.hum   = HumToSend;
    msg.env.temp2 = Temp2ToSend;
    msg.env.temp1 = Temp1ToSend;
    SendMsgToHost(&msg);
  }
}

#if SENSING1_USE_BATTERY
/**
  * @brief  Send Battery Info Data (Voltage/Current/Soc) to BLE
  * @param  None
  * @retval None
  */
static void SendBatteryInfoData(void)
{
  uint32_t soc_status;
  int32_t current= 0;

  msgData_t msg;
  uint32_t voltage;

#ifdef STM32_SENSORTILE
  uint8_t v_mode;
  /* Update Gas Gouge Status */
  BSP_GG_Task(TargetBoardFeatures.HandleGGComponent,&v_mode);

  /* Read the Gas Gouge Status */
  BSP_GG_GetVoltage(TargetBoardFeatures.HandleGGComponent, &voltage);
  BSP_GG_GetCurrent(TargetBoardFeatures.HandleGGComponent, &current);
  BSP_GG_GetSOC(TargetBoardFeatures.HandleGGComponent, &soc_status);
#else /* STM32_SENSORTILE */
  uint32_t BatteryLevel;

  /* Read the Battery Charger voltage value */
  BSP_BC_GetVoltageAndLevel(&voltage,&BatteryLevel);

  soc_status     = BatteryLevel; /* Unknown */
  current = 0x8000; /* No info for Current */
#endif /* STM32_SENSORTILE */

  /* Battery Informations */
  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_GG_EVENT)) {

    msg.type                   = BATTERY_INFO;
    msg.batteryInfo.soc_status = soc_status;
    msg.batteryInfo.current    = current;
    msg.batteryInfo.voltage    = voltage;

    SendMsgToHost(&msg);
  }
}
#endif /* SENSING1_USE_BATTERY*/


/** @brief Initialize the BlueNRG Stack
 * @param None
 * @retval None
 */
static void Init_BlueNRG_Stack(void)
{
  char BoardName[8];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  int ret;

  for(int i=0; i<7; i++) {
    BoardName[i]= NodeName[i+1];
  }

  BoardName[7]= 0;

#ifdef MAC_SENSING1
  {
    uint8_t tmp_bdaddr[6]= {MAC_SENSING1};
    int32_t i;
    for(i=0;i<6;i++) {
      bdaddr[i] = tmp_bdaddr[i];
    }
  }
#endif /* MAC_SENSING1 */

  /* Initialize the BlueNRG HCI */
#ifndef SENSING1_BlueNRG2
  hci_init(HCI_Event_CB, NULL);
  /* Reset BlueNRG hardware */
  hci_reset();
#else /* SENSING1_BlueNRG2 */
  hci_init(APP_UserEvtRx, NULL);
#endif /* SENSING1_BlueNRG2 */

#ifndef MAC_SENSING1
  #ifdef MAC_STM32UID_SENSING1
  /* Create a Unique BLE MAC Related to STM32 UID */
  {
    bdaddr[0] = (UID_BASE[1]>>24)&0xFF;
    bdaddr[1] = (UID_BASE[0]    )&0xFF;
    bdaddr[2] = (UID_BASE[2] >>8)&0xFF;
    bdaddr[3] = (UID_BASE[0]>>16)&0xFF;
    bdaddr[4] = (((SENSING1_VERSION_MAJOR-48)*10) + (SENSING1_VERSION_MINOR-48)+100)&0xFF;
    bdaddr[5] = 0xC0; /* for a Legal BLE Random MAC */
  }
  #else /* MAC_STM32UID_SENSING1 */
  {
    /* we will let the BLE chip to use its Random MAC address */
    uint8_t data_len_out;
#ifndef SENSING1_BlueNRG2
   ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, 6, &data_len_out, bdaddr);
#else /* SENSING1_BlueNRG2 */
  #define CONFIG_DATA_RANDOM_ADDRESS          (0x80) /**< Stored static random address. Read-only. */
  ret = aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, &data_len_out, bdaddr);
#endif /* SENSING1_BlueNRG2 */

    if(ret){
      SENSING1_PRINTF("\r\nReading  Random BD_ADDR failed\r\n");
      goto fail;
    }
  }
  #endif /* MAC_STM32UID_SENSING1 */
#else /* MAC_SENSING1 */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);

  if(ret){
     SENSING1_PRINTF("\r\nSetting Public BD_ADDR failed\r\n");
     goto fail;
  }
#endif /* MAC_SENSING1 */

  ret = aci_gatt_init();
  if(ret){
     SENSING1_PRINTF("\r\nGATT_Init failed\r\n");
     goto fail;
  }

#ifndef SENSING1_BlueNRG2
  ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
#else /* SENSING1_BlueNRG2 */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
#endif /* SENSING1_BlueNRG2 */


  if(ret != BLE_STATUS_SUCCESS){
     SENSING1_PRINTF("\r\nGAP_Init failed\r\n");
     goto fail;
  }

#ifndef  MAC_SENSING1
  #ifdef MAC_STM32UID_SENSING1
    ret = hci_le_set_random_address(bdaddr);

    if(ret){
       SENSING1_PRINTF("\r\nSetting the Static Random BD_ADDR failed\r\n");
       goto fail;
    }
  #endif /* MAC_STM32UID_SENSING1 */
#endif /* MAC_SENSING1 */

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   7/*strlen(BoardName)*/, (uint8_t *)BoardName);

  if(ret){
     SENSING1_PRINTF("\r\naci_gatt_update_char_value failed\r\n");
    while(1);
  }

#ifndef SENSING1_BlueNRG2
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL, 7, 16,
                                     USE_FIXED_PIN_FOR_PAIRING, 123456,
                                     BONDING);
#else /* SENSING1_BlueNRG2 */
  ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_REQUIRED,
                                               SC_IS_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7,
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456,
                                               0x00);
#endif /* SENSING1_BlueNRG2 */
  if (ret != BLE_STATUS_SUCCESS) {
     SENSING1_PRINTF("\r\nGAP setting Authentication failed\r\n");
     goto fail;
  }

  SENSING1_PRINTF("\r\nSERVER: BLE Stack Initialized \r\n"
         "\t\tBoardName= %s\r\n"
         "\t\tBoardMAC = %02x:%02x:%02x:%02x:%02x:%02x\r\n",
         BoardName,
         bdaddr[5],bdaddr[4],bdaddr[3],bdaddr[2],bdaddr[1],bdaddr[0]);

  /* Set output power level */
  aci_hal_set_tx_power_level(1,4); /* -2,1 dBm */

  return;

fail:
  return;
}

#ifdef SENSING1_BlueNRG2
void APP_UserEvtRx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if(hci_pckt->type == HCI_EVENT_PKT) {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(event_pckt->evt == EVT_LE_META_EVENT) {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++) {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code) {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    } else if(event_pckt->evt == EVT_VENDOR) {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++) {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code) {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    } else {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++) {
        if (event_pckt->evt == hci_events_table[i].evt_code) {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}
#endif /* SENSING1_BlueNRG2 */

/** @brief Initialize all the Custom BlueNRG services
 * @param None
 * @retval None
 */
static void Init_BlueNRG_Custom_Services(void)
{
  int ret;

  ret = Add_HW_SW_ServW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS) {
     SENSING1_PRINTF("HW & SW Service W2ST added successfully\r\n");
  } else {
     SENSING1_PRINTF("\r\nError while adding HW & SW Service W2ST\r\n");
  }

  ret = Add_ConsoleW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS) {
     SENSING1_PRINTF("Console Service W2ST added successfully\r\n");
  } else {
     SENSING1_PRINTF("\r\nError while adding Console Service W2ST\r\n");
  }

  ret = Add_ConfigW2ST_Service();
  if(ret == BLE_STATUS_SUCCESS) {
     SENSING1_PRINTF("Config  Service W2ST added successfully\r\n");
  } else {
     SENSING1_PRINTF("\r\nError while adding Config Service W2ST\r\n");
  }
}

#ifdef STM32_SENSORTILEBOX


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /**Configure the main internal regulator output voltage
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }

   /**Configure LSE Drive Capability
    */
  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|
                                     RCC_OSCILLATORTYPE_LSE  |
                                     RCC_OSCILLATORTYPE_HSE  |
                                     RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK   |
                                RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1  |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1   |
                                      RCC_PERIPHCLK_DFSDM1 |
                                      RCC_PERIPHCLK_USB    |
                                      RCC_PERIPHCLK_RTC    |
                                      RCC_PERIPHCLK_SDMMC1 |
                                      RCC_PERIPHCLK_ADC;

  PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK2;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLP;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 5;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 96;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV25;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK|RCC_PLLSAI1_SAI1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }
}

#else

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 48000000
 *            PLL_M                          = 6
 *            PLL_N                          = 40
 *            PLL_P                          = 7
 *            PLL_R                          = 4
 *            PLL_Q                          = 4
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
#if SENSING1_USE_USB
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
#endif /* SENSING1_USE_USB */

  /* Enable access to the backup domain */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /* RTC Clock selection can be changed only if the Backup Domain is reset */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();

  /* set low drive on LSE to reduce power consumption */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /* Enable the LSE Oscilator and Disable the LSI Oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  /* Enable the CSS interrupt in case LSE signal is corrupted or not present */
  HAL_RCCEx_DisableLSECSS();

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM            = 6;
  RCC_OscInitStruct.PLL.PLLN            = 40;
  RCC_OscInitStruct.PLL.PLLP            = 7;
  RCC_OscInitStruct.PLL.PLLQ            = 4;
  RCC_OscInitStruct.PLL.PLLR            = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  /* Enable MSI Auto-calibration through LSE */
  HAL_RCCEx_EnableMSIPLLMode();

#if SENSING1_USE_USB
  /* Select MSI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_MSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
#endif /* SENSING1_USE_USB */

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
                                      RCC_CLOCKTYPE_HCLK |
                                      RCC_CLOCKTYPE_PCLK1 |
                                      RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    while(1);
  }
}
#endif /* STM32_SENSORTILEBOX */

#ifdef USE_STM32L475E_IOT01
HAL_StatusTypeDef MX_DFSDM1_ClockConfig(DFSDM_Channel_HandleTypeDef *hDfsdmChannel, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hDfsdmChannel);

  HAL_StatusTypeDef        status = HAL_OK;
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

  /* Configure the SAI PLL according to the requested audio frequency */
  /* Retrieve actual RCC configuration */
  HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

  /* Set the PLL configuration according to the audio frequency */
  /* SAI1 clock config
  PLLSAI1_VCO = (48 MHz / PLLSAI1M) * PLLSAI1N = 48 / 6 * 43 = 344
  SAI_CK_x = PLLSAI1_VCO/PLLSAI1P = 2344 / 7 = 49.142 MHz */
  RCC_ExCLKInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_SAI1;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1M        = 6;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1N        = 43;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1P        = 7;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  RCC_ExCLKInitStruct.Sai1ClockSelection      = RCC_SAI1CLKSOURCE_PLLSAI1;
  status = HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);

  return status;
}
#endif /* USE_STM32L475E_IOT01 */

/**
  * @brief  Configure the current date.
  * @param  WeekDay Specifies the RTC Date WeekDay and it can be a value of @ref RTC_WeekDay_Definitions
  * @param  Date    Specifies the RTC Date Month (in BCD format) and it can be a value of @ref RTC_Month_Date_Definitions
  * @param  Month   Specifies the RTC Date and it must be a number between Min_Data = 1 and Max_Data = 31
  * @param  Year    Specifies the RTC Date Year and it must be a number between Min_Data = 0 and Max_Data = 99
  * @retval None
  */
void RTC_DateConfig(uint8_t WeekDay, uint8_t Date, uint8_t Month, uint8_t Year)
{
  RTC_DateTypeDef  sdatestructure;

  sdatestructure.WeekDay = WeekDay;
  sdatestructure.Date = Date;
  sdatestructure.Month = Month;
  sdatestructure.Year = Year;

  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK) {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  Configure the current time.
  * @param  Hours   Specifies the RTC Time Hour.
  *                 This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
  *                 This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected
  * @param  Minutes Specifies the RTC Time Minutes and it must be a number between Min_Data = 0 and Max_Data = 59
  * @param  Seconds Specifies the RTC Time Seconds and it must be a number between Min_Data = 0 and Max_Data = 59
  * @retval None
  */
void RTC_TimeConfig(uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
{
  RTC_TimeTypeDef  stimestructure;

  stimestructure.Hours = Hours;
  stimestructure.Minutes = Minutes;
  stimestructure.Seconds = Seconds;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BCD) != HAL_OK) {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  Get the current data and time value.
  * @param  None
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef RTC_GetCurrentDateTime(void)
{
  HAL_StatusTypeDef Status;
  /* Get the RTC current Time */
  Status = HAL_RTC_GetTime(&RtcHandle, &CurrentTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  Status = HAL_RTC_GetDate(&RtcHandle, &CurrentDate, RTC_FORMAT_BIN);
  return Status;
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note This is a user implementation using WFI state
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay){
    __WFI();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  SENSING1_PRINTF("Error_Handler\r\n");
  /* User may add here some code to deal with this error */
  while(1){
  }
}

/**
 * @brief  EXTI line detection callback.
 * @param  uint16_t GPIO_Pin Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  msgData_t msg;

  switch(GPIO_Pin){
    case HCI_TL_SPI_EXTI_PIN:
      /* Same define name for all the platforms*/
      hci_tl_lowlevel_isr();
      msg.type  = PROCESS_EVENT;
      SendMsgToHost(&msg);
    break;
#ifndef STM32_SENSORTILE
  #ifdef USE_STM32L4XX_NUCLEO
    case KEY_BUTTON_PIN:
  #elif defined(USE_STM32L475E_IOT01)
    case BUTTON_USER_PIN:
  #elif defined(STM32_SENSORTILEBOX)
    case USER_BUTTON_PIN:
  #else
    #error "Set the Right Platform"
  #endif /* USE_STM32L4XX_NUCLEO */
    ButtonPressed = 1;
    if(semRun) {
      osSemaphoreRelease(semRun);
    }
    break;
#endif /* STM32_SENSORTILE */

#ifdef STM32_SENSORTILEBOX
    case POWER_BUTTON_PIN:
      /* Power off the board */
      PowerButtonPressed = 1;
      if(semRun) {
        osSemaphoreRelease(semRun);
      }
    break;
#endif /* STM32_SENSORTILEBOX */

#ifndef USE_STM32L475E_IOT01
  /* The IoT01A1 has not the HW events */
  #ifdef USE_STM32L4XX_NUCLEO
      case GPIO_PIN_5:
      case GPIO_PIN_4:
  #elif defined(STM32_SENSORTILE)
    case BSP_LSM6DSM_INT2:
  #elif defined(STM32_SENSORTILEBOX)
      /* HW events from LSM6DSOX */
     case GPIO_PIN_3:
  #else /* USE_STM32L4XX_NUCLEO */
    #error "Set the Int pin for this platform"
  #endif /* USE_STM32L4XX_NUCLEO */
      MEMSInterrupt=1;
      if(semRun) {
        osSemaphoreRelease(semRun);
      }
      break;
#endif /* USE_STM32L475E_IOT01 */
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: SENSING1_PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1){
  }
}
#endif

/**
 * @brief  Check if there are a valid Node Name Values in Memory and read them
 * @param  None
 * @retval unsigned char Success/Not Success
 */
static unsigned char ReCallNodeNameFromMemory(void)
{
  const char DefaultBoardName[7] = {NAME_BLUEMS};

  /* ReLoad the Node Name Values from RAM */
  unsigned char Success=0;

  /* Recall the node name Credential saved */
  MDM_ReCallGMD(GMD_NODE_NAME,(void *)&NodeName);

  if(NodeName[0] != 0x12) {
    NodeName[0]= 0x12;

    for(int i=0; i<7; i++) {
      NodeName[i+1]= DefaultBoardName[i];
    }

    MDM_SaveGMD(GMD_NODE_NAME,(void *)&NodeName);
    NecessityToSaveMetaDataManager=1;
  }

  return Success;
}

void LedBlinkStart(void)
{
  ledTimer = LED_TIME_ON;
  LedOnTargetPlatform();
  if (!timLedId) {
    timLedId     = osTimerCreate (osTimer(TimerLedHandle),osTimerOnce, NULL);
  }
  if (timLedId){
    if  (osTimerStart (timLedId, ledTimer) != osOK) {
      SENSING1_PRINTF("failed starting timer\n\r");
    }
  }
}

void LedBlinkStop(void)
{
  LedOffTargetPlatform();
  if (timLedId) {
    if  (osTimerStop (timLedId) != osOK){
      SENSING1_PRINTF("could not stop led timer\n\r");
    }
    if (osTimerDelete (timLedId) != osOK)  {
      SENSING1_PRINTF("could not delete led timer\n\r");
    }
  timLedId = NULL;
  ledTimer = (uint32_t)NULL;
  }
}

static void LedBlinkCb  (void const *arg)
{
  if (ledTimer == LED_TIME_ON){
    ledTimer = LED_TIME_OFF;
    LedOffTargetPlatform();
  } else {
    ledTimer = LED_TIME_ON;
    LedOnTargetPlatform();
  }
  if (timLedId){
    if  (osTimerStart (timLedId, ledTimer) != osOK){
      SENSING1_PRINTF("failed starting timer\n\r");
    }
  }
}

void vApplicationStackOverflowHook (void)
{
  while (1)
  {

  }
}
/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
