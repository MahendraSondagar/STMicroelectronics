/**
  ******************************************************************************
  * @file    DataLog_Manager.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   SD Card Data Log Manager APIs implementation
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

/* Includes ------------------------------------------------------------------*/
#include "TargetFeatures.h"

#if SENSING1_USE_DATALOG
#include "sensor_service.h"
#include "DataLog_Manager.h"
#include "PowerControl.h"
#include "main.h"
#include "bsp.h"
/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio_SensorTile.box.h"
/* Imported Variables -------------------------------------------------------------*/
extern osSemaphoreId semRun;

/* Private Defines ---------------------------------------------------------------*/
#define MAX_TRIALS_OPENS_SD 10

/* Define the Max Lenght for MEMS/Audio Log File Name */
#define  SENSING1_MAX_LEN_LOG_FILE_NAME 64
#define AUDIO_BUFF_LEN (PCM_AUDIO_IN_SAMPLES *64)
#define AUDIO_BUFF_LEN_MASK (AUDIO_BUFF_LEN - 1)

/* Exported Variables -------------------------------------------------------------*/
volatile uint8_t writeAudio_flag=0;
volatile uint32_t NbAudioSamplesCounter;

/* Feature mask that identify the data mens selected for recording*/
uint32_t SD_Card_FeaturesMask= 0;
uint32_t SD_LogAudio_Enabled = 0;
uint32_t SD_LogMems_Enabled = 0;

/* Data File Name. 12 == 11 Max Chars from BLE + termination char */
char DefaultDataFileName[12];

uint32_t RoundedInertialWakeUpTimer = 10;   /* 10mSec */
uint16_t SampleRateIneFeatures      = 1040;  /* 104 Hz */

uint32_t RoundCounterEnvironmental  = 100; /* 1Sec */
uint32_t RoundedEnvironmentalFreq   = 10; /* 1Hz */

uint32_t NoSDFlag=1;

/* Private Variables -------------------------------------------------------------*/

const Diskio_drvTypeDef *DiskIO_Driver = &SD_Driver;

/* SD card logical drive path */
static char SDPath[4];
static uint8_t pAudioHeader[44];

static uint32_t IsSdMemsRecording= 0;
static uint32_t IsSdAudioRecording= 0;

/* Files object */
static FIL MyFileMems;
static FIL MyFileAudio;
FIL MyFileDummy;

/* File system object for SD card logical drive */
FATFS SDFatFs;

static int32_t  index_buff=0;

static char *MonthName[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static uint16_t *Audio_OUT_Buff = NULL;

/* Private Prototypes -------------------------------------------------------------*/
static uint32_t WavProcess_HeaderInit(void);
static uint32_t WavProcess_HeaderUpdate(uint32_t len);

static void CreateMemsFileName(char *FileName,uint32_t OnlyForAnnotation);
static void CreateAudioFileName(char *FileName);

static void openFileMems(uint32_t SomethingAlreadyRecording,uint32_t OnlyForAnnotation);
static void closeFileMems(uint32_t SomethingAlreadyRecording);

static void openFileAudio(uint32_t SomethingAlreadyRecording);
static void closeFileAudio(uint32_t SomethingAlreadyRecording);

static void SD_CardLoggingMemsData(void);

static uint8_t DATALOG_SD_LogMems_Enable(uint32_t SomethingAlreadyRecording,uint32_t OnlyForAnnotation);
static void DATALOG_SD_LogMems_Disable(uint32_t SomethingAlreadyRecording);

static uint8_t DATALOG_SD_LogAudio_Enable(uint32_t SomethingAlreadyRecording);
static void SaveAudioData(void);
static void DATALOG_SD_LogAudio_Disable(uint32_t SomethingAlreadyRecording);

/**
  * @brief  Management of the audio data logging
  * @param  pInBuff     points to the audio buffer.
  * @param  len         number of samples to process.
  * @retval None
  */
void AudioProcess_SD_Recording(uint16_t *pInBuff, uint32_t len)
{
  static uint16_t OutBuffIndex = 0;
  if(Audio_OUT_Buff!=NULL) {
    /* Accumulate audio buffer into local ping-pong buffer before writing */
    memcpy(Audio_OUT_Buff + OutBuffIndex, pInBuff, len * sizeof(uint16_t));
    OutBuffIndex += len;
    NbAudioSamplesCounter += len;

    OutBuffIndex &=AUDIO_BUFF_LEN_MASK;

    if(OutBuffIndex == (AUDIO_BUFF_LEN / 2)) {
      /* first half */
      index_buff=0;
      writeAudio_flag=1;
      if(semRun) {
        osSemaphoreRelease(semRun);
      }
    } else if (OutBuffIndex == 0) {
      /* second half */
      index_buff= AUDIO_BUFF_LEN / 2;
      writeAudio_flag=1;
      if(semRun) {
        osSemaphoreRelease(semRun);
      }
    }
  }
}

/**
  * @brief  Management of the audio file opening
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static void openFileAudio(uint32_t SomethingAlreadyRecording)
{
  if(NoSDFlag) {
    DATALOG_SD_Init();
    osDelay(200);
  }

  if(NoSDFlag==0) {
    if(Audio_OUT_Buff != NULL) {
      /* Free Memory for Audio Buffer */
      vPortFree((void*)Audio_OUT_Buff);
      Audio_OUT_Buff = NULL;
    }

    /* Allocate Memory for Audio Buffer */
    Audio_OUT_Buff  = (uint16_t * )pvPortMalloc(sizeof( uint16_t ) * AUDIO_BUFF_LEN);
    if (Audio_OUT_Buff == NULL) {
      SENSING1_PRINTF("Error: Failed to allocate memory for audio buffer.\r\n");
    }

    /* Reset received PCM samples counter */
    NbAudioSamplesCounter = 0;

    if(DATALOG_SD_LogAudio_Enable(SomethingAlreadyRecording)) {
      SD_LogAudio_Enabled=1;
    } else {
      DATALOG_SD_LogAudio_Disable(SomethingAlreadyRecording);
    }
  }
}

/**
  * @brief  Management of the audio file closing
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static void closeFileAudio(uint32_t SomethingAlreadyRecording)
{
  if(SD_LogAudio_Enabled) {
    DATALOG_SD_LogAudio_Disable(SomethingAlreadyRecording);
    SD_LogAudio_Enabled=0;
  }

  /* Free Memory for Audio Buffer */
  vPortFree((void*)Audio_OUT_Buff);
  Audio_OUT_Buff = NULL;
}

/**
  * @brief  Management of the MEMS data logging
  * @param  None
  * @retval None
  */
static void SD_CardLoggingMemsData(void)
{
  int32_t status;

  static int32_t CounterEnviromental=0;
  int32_t NeedToSaveSomething=0;

  char myBuffer[256];
  uint32_t CharPos=0;

  CounterEnviromental++;

  RTC_GetCurrentDateTime();
  CharPos = sprintf(myBuffer, "%02d:%02d:%02d.%03ld,",
                         CurrentTime.Hours,
                         CurrentTime.Minutes,
                         CurrentTime.Seconds,
                         999- (CurrentTime.SubSeconds*1000)/(CurrentTime.SecondFraction));

  /* Read Sensors and fill the output buffer */

  /* Inertial Features */
  /* Read Acc */
  if(SD_Card_FeaturesMask & FEATURE_MASK_ACC) {
    MOTION_SENSOR_Axes_t Acceleration;
    NeedToSaveSomething=1;
    status= MOTION_SENSOR_GetAxes(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO, &Acceleration);
    if (status == BSP_ERROR_NONE) {
      CharPos += sprintf(myBuffer+CharPos,",%ld,%ld,%ld",
                         Acceleration.x,
                         Acceleration.y,
                         Acceleration.z);
    } else {
      CharPos += sprintf(myBuffer+CharPos,"%s",",,,");
    }
  }

  /* Read Gyro */
  if(SD_Card_FeaturesMask & FEATURE_MASK_GRYO) {
    MOTION_SENSOR_Axes_t AngularVelocity;
    NeedToSaveSomething=1;
    status= MOTION_SENSOR_GetAxes( TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO, &AngularVelocity );
    if (status == BSP_ERROR_NONE) {
      CharPos += sprintf(myBuffer+CharPos,",%ld,%ld,%ld",
                         AngularVelocity.x,
                         AngularVelocity.y,
                         AngularVelocity.z);
    } else {
      CharPos += sprintf(myBuffer+CharPos,"%s",",,,");
    }
  }

  /* Read Mag */
  if(SD_Card_FeaturesMask & FEATURE_MASK_MAG) {
    MOTION_SENSOR_Axes_t Magnetometer;
    NeedToSaveSomething=1;
    status= MOTION_SENSOR_GetAxes( TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO, &Magnetometer );

    if ( status == BSP_ERROR_NONE ) {
      CharPos += sprintf(myBuffer+CharPos,",%ld,%ld,%ld",
                         Magnetometer.x,
                         Magnetometer.y,
                         Magnetometer.z);
    } else {
      CharPos += sprintf(myBuffer+CharPos,"%s",",,,");
    }
  }

  /* Environmental Features */
  if(CounterEnviromental==RoundCounterEnvironmental) {
    CounterEnviromental=0;

    /* Read Press */
    if(SD_Card_FeaturesMask & FEATURE_MASK_PRESS) {
      float Pressure;
      NeedToSaveSomething=1;
      if(TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
        status= ENV_SENSOR_GetValue( TargetBoardFeatures.HandlePressSensor, ENV_PRESSURE, &Pressure );
#ifdef ONE_SHOT
        if((SD_Card_FeaturesMask & FEATURE_MASK_TEMP2)==0) {
          /* For Making only one time for each Sensor */
          ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandlePressSensor);
        }
#endif
        if ( status == BSP_ERROR_NONE ) {
          CharPos += sprintf(myBuffer+CharPos,",%.2f",Pressure);
        } else {
          CharPos += sprintf(myBuffer+CharPos,"%c",',');
        }
      }
    }

    /* Read Temp1 */
    if(SD_Card_FeaturesMask & FEATURE_MASK_TEMP1) {
      float Temperature;
      NeedToSaveSomething=1;
      if(TargetBoardFeatures.HandleTempSensors[0] != SENSING1_SNS_NOT_VALID) {
        status= ENV_SENSOR_GetValue( TargetBoardFeatures.HandleTempSensors[0], ENV_TEMPERATURE, &Temperature);
#ifdef ONE_SHOT
        if((SD_Card_FeaturesMask & FEATURE_MASK_HUM)==0) {
          /* For Making only one time for each Sensor */
          ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandleTempSensors[0]);
        }
#endif
        if ( status == BSP_ERROR_NONE ) {
          CharPos += sprintf(myBuffer+CharPos,",%.2f",Temperature);
        } else {
          CharPos += sprintf(myBuffer+CharPos,"%c",',');
        }
      }
    }

    /* Read Temp2 */
    if(SD_Card_FeaturesMask & FEATURE_MASK_TEMP2) {
      float Temperature;
      NeedToSaveSomething=1;
      if(TargetBoardFeatures.HandleTempSensors[1] != SENSING1_SNS_NOT_VALID) {
        status= ENV_SENSOR_GetValue( TargetBoardFeatures.HandleTempSensors[1], ENV_TEMPERATURE, &Temperature);
#ifdef ONE_SHOT
        ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandleTempSensors[1]);
#endif
        if ( status == BSP_ERROR_NONE ) {
          CharPos += sprintf(myBuffer+CharPos,",%.2f",Temperature);
        } else {
          CharPos += sprintf(myBuffer+CharPos,"%c",',');
        }
      }
    }

    /* Read Hum */
    if(SD_Card_FeaturesMask & FEATURE_MASK_HUM) {
      float Humidity;
      NeedToSaveSomething=1;
      if(TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
        status= ENV_SENSOR_GetValue( TargetBoardFeatures.HandleHumSensor, ENV_HUMIDITY, &Humidity );
#ifdef ONE_SHOT
        ENV_SENSOR_Set_One_Shot(TargetBoardFeatures.HandleHumSensor);
#endif
        if ( status == BSP_ERROR_NONE ) {
          CharPos += sprintf(myBuffer+CharPos,",%.2f",Humidity);
        } else {
          CharPos += sprintf(myBuffer+CharPos,"%c",',');
        }
      }
    }
  } else {
    /* Put Trailing ,s */

    if(SD_Card_FeaturesMask & FEATURE_MASK_PRESS) {
      CharPos += sprintf(myBuffer+CharPos,"%c",',');
    }

    if(SD_Card_FeaturesMask & FEATURE_MASK_TEMP1) {
      CharPos += sprintf(myBuffer+CharPos,"%c",',');
    }

    if(SD_Card_FeaturesMask & FEATURE_MASK_TEMP2) {
      CharPos += sprintf(myBuffer+CharPos,"%c",',');
    }

    if(SD_Card_FeaturesMask & FEATURE_MASK_HUM) {
        CharPos += sprintf(myBuffer+CharPos,"%c",',');
    }
  }

  /* Termination & Write on File */
  if((SD_LogMems_Enabled!=0) & (NeedToSaveSomething==1)){
    uint32_t byteswritten;
    CharPos += sprintf(myBuffer+CharPos,"%c",'\n');
    if(f_write(&MyFileMems, (const void*)myBuffer, CharPos, (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
    }
  }
}

/**
  * @brief  Management of the file opening
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @param uint32_t OnlyForAnnotation Flag for understanding if the file is
  *   created only for Annotation purpose
  * @retval None
  */
static void openFileMems(uint32_t SomethingAlreadyRecording,uint32_t OnlyForAnnotation)
{
  if(NoSDFlag) {
    DATALOG_SD_Init();
    osDelay(200);
  }
  if(NoSDFlag==0) {
    if(DATALOG_SD_LogMems_Enable(SomethingAlreadyRecording,OnlyForAnnotation)) {
      SD_LogMems_Enabled=1;
    } else {
      DATALOG_SD_LogMems_Disable(SomethingAlreadyRecording);
    }
  }
}

/**
  * @brief  Management of the file closing
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static void closeFileMems(uint32_t SomethingAlreadyRecording)
{
  if(SD_LogMems_Enabled) {
    DATALOG_SD_LogMems_Disable(SomethingAlreadyRecording);
    SD_LogMems_Enabled=0;
  }
}

/**
 * @brief  Management of the start of the SD Card MEMS data logging
 * @param uint32_t OnlyForAnnotation Flag for understanding if the file is
 *   created only for Annotation purpose
 * @retval None
 */
void SD_CardLoggingMemsStart(uint32_t OnlyForAnnotation)
{
  if(!IsSdMemsRecording) {
    LedOffTargetPlatform();
    openFileMems(IsSdAudioRecording,OnlyForAnnotation);

     osDelay(100);

    if(SD_LogMems_Enabled) {
      IsSdMemsRecording= 1;

      if(OnlyForAnnotation==0) {
        /* Call the SD-Log every RoundedInertialWakeUpTimer [mS] */
        startProc(SD_CARD_LOGGING,RoundedInertialWakeUpTimer);

        if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
          BytesToWrite =sprintf((char *)BufferToWrite,"Start Data Log MEMS Rec\n");
          Term_Update(BufferToWrite,BytesToWrite);
        }

        PowerCtrlLock();
        if( (SD_Card_FeaturesMask & FEATURE_MASK_ACC)  ||
            (SD_Card_FeaturesMask & FEATURE_MASK_GRYO) ||
            (SD_Card_FeaturesMask & FEATURE_MASK_MAG) ) {
          EnableMotionSensors ();


          /* Change Default ODR for Acc/Gyro/Mag.
           * The References ODR are the one for Acc/Gyro:
           * Acc/Gyro  13Hz -> Mag =  10Hz
           * Acc/Gyro  26Hz -> Mag =  20Hz
           * Acc/Gyro  52Hz -> Mag =  50Hz
           * Acc/Gyro 104Hz -> Mag = 100Hz */
          if(SD_Card_FeaturesMask & FEATURE_MASK_ACC) {
            /* set default full scale */ 
            Set2GAccelerometerFullScale();

            MOTION_SENSOR_GetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,&TargetBoardFeatures.DefaultAccODR);

            if(SampleRateIneFeatures <= 130) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,13.0f);
            } else if (SampleRateIneFeatures <= 260) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,26.0f);
            } else if (SampleRateIneFeatures <= 520) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,52.0f);
            } else {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,104.0f);
            }
          }

          if(SD_Card_FeaturesMask & FEATURE_MASK_GRYO) {
            MOTION_SENSOR_GetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,&TargetBoardFeatures.DefaultGyroODR);

            if(SampleRateIneFeatures <= 130) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,13.0f);
            } else if (SampleRateIneFeatures <= 260) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,26.0f);
            } else if (SampleRateIneFeatures <= 520) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,52.0f);
            } else {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,104.0f);
            }
          }

          if(SD_Card_FeaturesMask & FEATURE_MASK_MAG) {
            MOTION_SENSOR_GetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,&TargetBoardFeatures.DefaultMagODR);

            if(SampleRateIneFeatures <= 130) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,10.0f);
            } else if (SampleRateIneFeatures <= 260) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,20.0f);
            } else if (SampleRateIneFeatures <= 520) {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,50.0f);
            } else {
              MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,100.0f);
            }
          }
        }

        if( (SD_Card_FeaturesMask & FEATURE_MASK_PRESS) ||
            (SD_Card_FeaturesMask & FEATURE_MASK_HUM)   ||
            (SD_Card_FeaturesMask & FEATURE_MASK_TEMP1) ||
            (SD_Card_FeaturesMask & FEATURE_MASK_TEMP2) ) {
            EnableEnvSensors ();
        }
      }
    } else {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
        BytesToWrite =sprintf((char *)BufferToWrite,"1 SD Card not present\n");
        Stderr_Update(BufferToWrite,BytesToWrite);
      }
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_NO_SD);
      }
      NoSDFlag = 1;
    }
  } else {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite,"2 Data Log MEMS is already started\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    }
  }
}


/**
 * @brief  Management of the start of the SD Card Audio data logging
 * @param  None
 * @retval None
 */
void SD_CardLoggingAudioStart(void)
{
  if(!IsSdAudioRecording) {
    LedOffTargetPlatform();
    openFileAudio(IsSdMemsRecording);

    osDelay(100);

    if(SD_LogAudio_Enabled) {
      IsSdAudioRecording= 1;
      InitMics(AUDIO_SAMPLING_FREQUENCY);

      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
        BytesToWrite =sprintf((char *)BufferToWrite,"Start Data Log Audio Rec\n");
        Term_Update(BufferToWrite,BytesToWrite);
      }
    } else {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
        BytesToWrite =sprintf((char *)BufferToWrite,"3 SD Card not present\n");
        Stderr_Update(BufferToWrite,BytesToWrite);
      }
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_NO_SD);
      }
      NoSDFlag=1;
    }
  } else {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite,"4 Data Log Audio is already started\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    }
  }
}

/**
 * @brief  Management of the stop of the SD Card MEMS data logging
 * @param  None
 * @retval None
 */
void SD_CardLoggingMemsStop(void)
{
  if(IsSdMemsRecording) {
    /* Stop Timer for SD Card Logging  */
    stopProc(SD_CARD_LOGGING);

    IsSdMemsRecording= 0;

    closeFileMems(IsSdAudioRecording);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"Stop Data Log MEMS Rec\n");
      Term_Update(BufferToWrite,BytesToWrite);
    }

    if((SD_Card_FeaturesMask & FEATURE_MASK_ACC)  ||
      (SD_Card_FeaturesMask & FEATURE_MASK_GRYO) ||
      (SD_Card_FeaturesMask & FEATURE_MASK_MAG) ) {

      /* Restor Default ODR for Acc/Gyro/Mag */
      if(SD_Card_FeaturesMask & FEATURE_MASK_ACC) {
        MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO,TargetBoardFeatures.DefaultAccODR);
      }

      if(SD_Card_FeaturesMask & FEATURE_MASK_GRYO) {
        MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleGyroSensor, MOTION_GYRO,TargetBoardFeatures.DefaultGyroODR);
      }

      if(SD_Card_FeaturesMask & FEATURE_MASK_MAG) {
        MOTION_SENSOR_SetOutputDataRate(TargetBoardFeatures.HandleMagSensor, MOTION_MAGNETO,TargetBoardFeatures.DefaultMagODR);
      }
      DisableMotionSensors();
    }

    if( (SD_Card_FeaturesMask & FEATURE_MASK_PRESS) ||
        (SD_Card_FeaturesMask & FEATURE_MASK_HUM)   ||
        (SD_Card_FeaturesMask & FEATURE_MASK_TEMP1) ||
        (SD_Card_FeaturesMask & FEATURE_MASK_TEMP2) ) {
      DisableEnvSensors ();
    }
  } else {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite,"5 None Data Log MEMS\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    }
  }
  PowerCtrlUnLock();
  LedInitTargetPlatform();
}

/**
 * @brief  Management of the stop of the SD Card Audio data logging
 * @param  None
 * @retval None
 */
void SD_CardLoggingAudioStop(void)
{
  if(IsSdAudioRecording) {
    DeInitMics();

    writeAudio_flag=0;
    IsSdAudioRecording= 0;

    closeFileAudio(IsSdMemsRecording);

    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"Stop Data Log Audio Rec\n");
      Term_Update(BufferToWrite,BytesToWrite);
    }
  } else {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite,"6 None Data Log Audio\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    }
  }
  PowerCtrlUnLock();
  LedInitTargetPlatform();
}


/**
 * @brief  SD Card Mems logging run
 * @param uint32_t OnlyForAnnotation Flag for understanding if the file is
 *   created only for Annotation purpose
 * @retval None
 */
void SdCardMemsRecordingRun(uint32_t OnlyForAnnotation)
{
  if(!IsSdMemsRecording) {
    SD_CardLoggingMemsStart(OnlyForAnnotation);
  }

  if(IsSdMemsRecording) {
     SD_CardLoggingMemsData();
  }
  LedInitTargetPlatform();
  LedOffTargetPlatform();
}

/**
 * @brief  SD Card Audio logging run
 * @param  None
 * @retval None
 */
void SdCardAudioRecordingRun(void)
{
  if(!IsSdAudioRecording) {
    SD_CardLoggingAudioStart();
  }

  if(IsSdAudioRecording) {
     SaveAudioData();
  }
  LedInitTargetPlatform();
  LedOffTargetPlatform();
}

/**
  * @brief  Start SD-Card demo
  * @param  None
  * @retval None
  */
void DATALOG_SD_Init(void)
{
  if (FATFS_LinkDriver(DiskIO_Driver, SDPath) != 0)
  {
    BSP_LED_On(LED_BLUE);
  }

   /* Register the file system object to the FatFs module */
  if (f_mount(&SDFatFs, (TCHAR const *)SDPath, 0) != FR_OK)
  {
    BSP_LED_On(LED_BLUE);
  }

  /* Explicit Init to trap SD Missing */
  if (DiskIO_Driver->disk_initialize(0) != RES_OK) {
    NoSDFlag=1;
    BSP_LED_Off(LED_GREEN);
    DATALOG_SD_DeInit();
  } else {
    NoSDFlag=0;
    BSP_LED_On(LED_GREEN);
  }
}

/**
  * @brief  Start SD-Card demo
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @param uint32_t OnlyForAnnotation Flag for understanding if the file is
  *   created only for Annotation purpose
  * @retval None
  */
static uint8_t DATALOG_SD_LogMems_Enable(uint32_t SomethingAlreadyRecording, uint32_t OnlyForAnnotation)
{
  uint32_t byteswritten;
  uint32_t SDCardFileCount = 0;
  char MemsDataFileName[SENSING1_MAX_LEN_LOG_FILE_NAME];

  /*Create the file */
  CreateMemsFileName(MemsDataFileName,OnlyForAnnotation);

  while(SD_LogMems_Enabled==0) {
    SDCardFileCount++;

    if(f_open(&MyFileMems, (char const*)MemsDataFileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
      if(SDCardFileCount > MAX_TRIALS_OPENS_SD) {
        SDCardFileCount = 0;
        SD_LogMems_Enabled= 0;
        return 0;
      }
      osDelay(100);
    } else {
    SD_LogMems_Enabled=1;
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite, "FileName=%s\n",MemsDataFileName);
      Term_Update(BufferToWrite,BytesToWrite);
    }
  }
  }

  if(OnlyForAnnotation) {
    /* .csv header */
    uint8_t pHeader[]= "hh:mm:ss.ms, Annotation\n";
    char Introduction[64];
    uint32_t CharPos=0;

    /* If we are here... there is only Mic acquisition */
    CharPos = sprintf(Introduction,"Sensors' Acquisition [Hz] setup: Mic@%d Volume=%ld\n",AUDIO_SAMPLING_FREQUENCY,TargetBoardFeatures.AudioVolume);

    if(f_write(&MyFileMems, (const void*)Introduction, CharPos, (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
      return 0;
    }

    /* Write the header */
    if(f_write(&MyFileMems, (const void*)pHeader, sizeof(pHeader), (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
      return 0;
    }
  } else {
    /* .csv header */
    char Header[256];
    char Introduction[256];
    uint32_t CharPosIntro=0;
    uint32_t CharPosHeader=0;
    int32_t IneHz = SampleRateIneFeatures/10;
    int32_t IneSubHz = SampleRateIneFeatures%10;
    int32_t EnvHz = RoundedEnvironmentalFreq/10;
    int32_t EnvSubHz = RoundedEnvironmentalFreq%10;

    CharPosHeader = sprintf(Header,"%s","hh:mm:ss.ms, Annotation ");
    CharPosIntro = sprintf(Introduction,"%s","Sensors' Acquisition [Hz]: ");

    /* Microphone */
    if(SD_Card_FeaturesMask&FEATURE_MASK_BLUEVOICE) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"Mic@%d Volume=%ld ",AUDIO_SAMPLING_FREQUENCY,TargetBoardFeatures.AudioVolume);
    }

    /* Acc */
    if(SD_Card_FeaturesMask&FEATURE_MASK_ACC) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"Acc@%ld.%ld ",IneHz,IneSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", AccX [mg], AccY, AccZ");
    }

    /* Gyro */
    if(SD_Card_FeaturesMask&FEATURE_MASK_GRYO) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"Gyro@%ld.%ld ",IneHz,IneSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", GyroX [mdps], GyroY, GyroZ");
    }

    /* Mag */
    if(SD_Card_FeaturesMask&FEATURE_MASK_MAG) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"Mag@%ld.%ld ",IneHz,IneSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", MagX [mgauss], MagY, MagZ");
    }

    /* Pressure */
    if(SD_Card_FeaturesMask&FEATURE_MASK_PRESS) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"P@%ld.%ld ",EnvHz,EnvSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", P [mB]");
    }

    /* Temperature1 */
    if(SD_Card_FeaturesMask&FEATURE_MASK_TEMP1) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"T1@%ld.%ld ",EnvHz,EnvSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", T1 ['C]");
    }

    /* Temperature2 */
    if(SD_Card_FeaturesMask&FEATURE_MASK_TEMP2) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"T2@%ld.%ld ",EnvHz,EnvSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", T2 ['C]");
    }

    /* Humidity */
    if(SD_Card_FeaturesMask&FEATURE_MASK_HUM) {
      CharPosIntro += sprintf(Introduction+CharPosIntro,"H@%ld.%ld ",EnvHz,EnvSubHz);
      CharPosHeader += sprintf(Header+CharPosHeader,"%s",", H [%]");
    }

    /* Termination */
    CharPosIntro += sprintf(Introduction+CharPosIntro,"%c",'\n');

    CharPosHeader += sprintf(Header+CharPosHeader,"%c",'\n');

    if(f_write(&MyFileMems, (const void*)Introduction, CharPosIntro, (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
      return 0;
    }

    /* Write the header */
    if(f_write(&MyFileMems, (const void*)Header, CharPosHeader, (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
      return 0;
    }
  }

  return 1;
}

/**
  * @brief  Start SD-Card demo
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static uint8_t DATALOG_SD_LogAudio_Enable(uint32_t SomethingAlreadyRecording)
{
  uint32_t SDCardFileCount = 0;
  uint32_t byteswritten;
  char AudioDataFileName[SENSING1_MAX_LEN_LOG_FILE_NAME];

  WavProcess_HeaderInit();

  /* SensorTile Audio Logging File Name */
  CreateAudioFileName(AudioDataFileName);

  while(SD_LogAudio_Enabled==0) {
    SDCardFileCount++;

    if(f_open(&MyFileAudio, (char const*)AudioDataFileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
      if(SDCardFileCount > MAX_TRIALS_OPENS_SD) {
        SDCardFileCount = 0;
        SD_LogAudio_Enabled= 0;
        return 0;
      }
      osDelay(100);
    } else {
      SD_LogAudio_Enabled =1;
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
        BytesToWrite =sprintf((char *)BufferToWrite, "FileName=%s\n",AudioDataFileName);
        Term_Update(BufferToWrite,BytesToWrite);
      }
    }
  }

  if(f_write(&MyFileAudio, (uint8_t*) pAudioHeader, sizeof(pAudioHeader), (void *)&byteswritten) != FR_OK) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
      SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
    }
    return 0;
  }

  /* Flush the cached information (pAudioHeader), writing data to the volume */
  if(f_sync(&MyFileAudio) != FR_OK) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
      SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
    }
    return 0;
  }

  PowerCtrlLock();
  return 1;
}

/**
  * @brief  .
  * @param  None
  * @retval None
  */
void DATALOG_SD_DeInit(void)
{
  FATFS_UnLinkDriver(SDPath);
  HAL_SD_DeInit(&hsd1);
}

/**
  * @brief  Disable SDCard Log
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static void DATALOG_SD_LogMems_Disable(uint32_t SomethingAlreadyRecording)
{
  if(SD_LogMems_Enabled) {
    f_close(&MyFileMems);
    SD_LogMems_Enabled =0;
  }

  if(SomethingAlreadyRecording==0) {
    LedOffTargetPlatform();
  }
}

/**
  * @brief  Disable SDCard Log
  * @param  uint32_t SomethingAlreadyRecording System already Initialized for SD recording
  * @retval None
  */
static void DATALOG_SD_LogAudio_Disable(uint32_t SomethingAlreadyRecording)
{
  uint32_t len;
  uint32_t byteswritten;

  if(SD_LogAudio_Enabled) {
    len = f_size(&MyFileAudio);
    WavProcess_HeaderUpdate(len);

    osDelay(100);

    /* Update the data length in the header of the recorded Wave */
    f_lseek(&MyFileAudio, 0);
    if(f_write(&MyFileAudio, (uint8_t*)pAudioHeader,  sizeof(pAudioHeader), (void*)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
    }

    /* Add a little Delay just in order to be sure to write the header before close the file */
    osDelay(100);

    /* Close file */
    f_close(&MyFileAudio);
    SD_LogAudio_Enabled=0;
  }

  if(SomethingAlreadyRecording==0 ) {
    LedOffTargetPlatform();
  }
}

void SaveDataAnnotation(uint8_t *Annotation)
{
  if(SD_LogMems_Enabled) {
    uint32_t byteswritten;
    uint32_t size = 0;
    char myBuffer[64];

     RTC_GetCurrentDateTime();
    /* Write data to the file on the SDCard */
    size = sprintf(myBuffer, "%02d:%02d:%02d.%03ld,%s",
                        CurrentTime.Hours,
                        CurrentTime.Minutes,
                        CurrentTime.Seconds,
                        999- (CurrentTime.SubSeconds*1000)/(CurrentTime.SecondFraction),
                        Annotation);
    /* Add the Traling , */
    if(SD_Card_FeaturesMask!=FEATURE_MASK_BLUEVOICE) {

      /* Acc */
      if(SD_Card_FeaturesMask&FEATURE_MASK_ACC) {
        size += sprintf(myBuffer+size, "%s",",,,");
      }

      /* Gyro */
      if(SD_Card_FeaturesMask&FEATURE_MASK_GRYO) {
        size += sprintf(myBuffer+size, "%s",",,,");
      }

      /* Mag */
      if(SD_Card_FeaturesMask&FEATURE_MASK_MAG) {
        size += sprintf(myBuffer+size, "%s",",,,");
      }

      /* Pressure */
      if(SD_Card_FeaturesMask&FEATURE_MASK_PRESS) {
        size += sprintf(myBuffer+size, "%c",',');
      }

      /* Temperature1 */
      if(SD_Card_FeaturesMask&FEATURE_MASK_TEMP1) {
        size += sprintf(myBuffer+size, "%c",',');
      }

      /* Temperature2 */
      if(SD_Card_FeaturesMask&FEATURE_MASK_TEMP2) {
        size += sprintf(myBuffer+size, "%c",',');
      }

      /* Humidity */
      if(SD_Card_FeaturesMask&FEATURE_MASK_HUM) {
        size += sprintf(myBuffer+size, "%c",',');
      }
    }

    /* Termination */
    size += sprintf(myBuffer+size, "%c",'\n');

    if(f_write(&MyFileMems, myBuffer, size, (void *)&byteswritten) != FR_OK) {
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
        SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
      }
    }
  }
}

static void SaveAudioData(void)
{
  uint32_t byteswritten;

  /* Skip first few sample to ignore init glitches (wait for audio signal to stabilize) */
  /* Skip first 100 ms */
  if (NbAudioSamplesCounter < 100 * 16) {
    return;
  }

  if(f_write(&MyFileAudio, ((uint8_t *)(Audio_OUT_Buff+index_buff)), AUDIO_BUFF_LEN /* Because we need to write 16bit for sample */, (void *)&byteswritten) != FR_OK) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
      SDLog_Update(SD_CARD_LOGGING_IO_ERROR);
    }
  }
}

/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderInit(void)
{
  uint16_t   BitPerSample=16;
  uint32_t   ByteRate=AUDIO_SAMPLING_FREQUENCY*(BitPerSample/8);

  uint32_t   SampleRate=AUDIO_SAMPLING_FREQUENCY;
  uint16_t   BlockAlign= BitPerSample/8;

  /* Write chunkID, must be 'RIFF'  ------------------------------------------*/
  pAudioHeader[0] = 'R';
  pAudioHeader[1] = 'I';
  pAudioHeader[2] = 'F';
  pAudioHeader[3] = 'F';

  /* Write the file length ----------------------------------------------------*/
  /* The sampling time: this value will be be written back at the end of the 
     recording opearation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pAudioHeader[4] = 0x00;
  pAudioHeader[5] = 0x4C;
  pAudioHeader[6] = 0x1D;
  pAudioHeader[7] = 0x00;

  /* Write the file format, must be 'WAVE' -----------------------------------*/
  pAudioHeader[8]  = 'W';
  pAudioHeader[9]  = 'A';
  pAudioHeader[10] = 'V';
  pAudioHeader[11] = 'E';

  /* Write the format chunk, must be'fmt ' -----------------------------------*/
  pAudioHeader[12]  = 'f';
  pAudioHeader[13]  = 'm';
  pAudioHeader[14]  = 't';
  pAudioHeader[15]  = ' ';

  /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
  pAudioHeader[16]  = 0x10;
  pAudioHeader[17]  = 0x00;
  pAudioHeader[18]  = 0x00;
  pAudioHeader[19]  = 0x00;

  /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
  pAudioHeader[20]  = 0x01;
  pAudioHeader[21]  = 0x00;

  /* Write the number of channels, ie. 0x01 (Mono) ---------------------------*/
  pAudioHeader[22]  = 1;
  pAudioHeader[23]  = 0x00;

  /* Write the Sample Rate in Hz ---------------------------------------------*/
  /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
  pAudioHeader[24]  = (uint8_t)((SampleRate & 0xFF));
  pAudioHeader[25]  = (uint8_t)((SampleRate >> 8) & 0xFF);
  pAudioHeader[26]  = (uint8_t)((SampleRate >> 16) & 0xFF);
  pAudioHeader[27]  = (uint8_t)((SampleRate >> 24) & 0xFF);

  /* Write the Byte Rate -----------------------------------------------------*/
  pAudioHeader[28]  = (uint8_t)(( ByteRate & 0xFF));
  pAudioHeader[29]  = (uint8_t)(( ByteRate >> 8) & 0xFF);
  pAudioHeader[30]  = (uint8_t)(( ByteRate >> 16) & 0xFF);
  pAudioHeader[31]  = (uint8_t)(( ByteRate >> 24) & 0xFF);

  /* Write the block alignment -----------------------------------------------*/
  pAudioHeader[32]  = BlockAlign;
  pAudioHeader[33]  = 0x00;

  /* Write the number of bits per sample -------------------------------------*/
  pAudioHeader[34]  = BitPerSample;
  pAudioHeader[35]  = 0x00;

  /* Write the Data chunk, must be 'data' ------------------------------------*/
  pAudioHeader[36]  = 'd';
  pAudioHeader[37]  = 'a';
  pAudioHeader[38]  = 't';
  pAudioHeader[39]  = 'a';

  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  pAudioHeader[40]  = 0x00;
  pAudioHeader[41]  = 0x4C;
  pAudioHeader[42]  = 0x1D;
  pAudioHeader[43]  = 0x00;

  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderUpdate(uint32_t len)
{
  /* Write the file length ----------------------------------------------------*/
  /* The sampling time: this value will be be written back at the end of the
     recording opearation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pAudioHeader[4] = (uint8_t)(len);
  pAudioHeader[5] = (uint8_t)(len >> 8);
  pAudioHeader[6] = (uint8_t)(len >> 16);
  pAudioHeader[7] = (uint8_t)(len >> 24);
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  len -=44;
  pAudioHeader[40] = (uint8_t)(len); 
  pAudioHeader[41] = (uint8_t)(len >> 8);
  pAudioHeader[42] = (uint8_t)(len >> 16);
  pAudioHeader[43] = (uint8_t)(len >> 24); 
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Create file name for mems data loggimg.
  * @param  FileName Name of the create file
  * @param uint32_t OnlyForAnnotation Flag for understanding if the file is
  *   created only for Annotation purpose
  * @retval None
  */
static void CreateMemsFileName(char *FileName,uint32_t OnlyForAnnotation)
{
  RTC_GetCurrentDateTime();
  if(OnlyForAnnotation) {
    sprintf(FileName, "%s-Ann_%02d_%s_%02d_%02dh_%02dm_%02ds.csv",
                       DefaultDataFileName,
                       CurrentDate.Date,
                       MonthName[CurrentDate.Month-1],
                       CurrentDate.Year,
                       CurrentTime.Hours,
                       CurrentTime.Minutes,
                       CurrentTime.Seconds);
  } else {
    sprintf(FileName, "%s-MemsAnn_%02d_%s_%02d_%02dh_%02dm_%02ds.csv",
                       DefaultDataFileName,
                       CurrentDate.Date,
                       MonthName[CurrentDate.Month-1],
                       CurrentDate.Year,
                       CurrentTime.Hours,
                       CurrentTime.Minutes,
                       CurrentTime.Seconds);
  }
}

/**
* @brief  Create file name for Audio data loggimg.
* @param  FileName Name of the create file
* @retval None
*/
static void CreateAudioFileName(char *FileName)
{
  RTC_GetCurrentDateTime();
  sprintf(FileName, "%s-Audio_%02d_%s_%02d_%02dh_%02dm_%02ds.wav",
                     DefaultDataFileName,
                     CurrentDate.Date,
                     MonthName[CurrentDate.Month-1],
                     CurrentDate.Year,
                     CurrentTime.Hours,
                     CurrentTime.Minutes,
                     CurrentTime.Seconds);
}

/**
 * @brief  Gets Time from RTC for FatFs
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void)
{
    DWORD fat_time;
    /* Get local time */
    RTC_GetCurrentDateTime();

    /* Pack date and time into a DWORD variable */
    fat_time =   ((DWORD)(CurrentDate.Year + 2000 - 1980) << 25)
               | ((DWORD)CurrentDate.Month << 21)
               | ((DWORD)CurrentDate.Date << 16)
               | ((DWORD)CurrentTime.Hours << 11)
               | ((DWORD)CurrentTime.Minutes << 5)
               | ((DWORD)CurrentTime.Seconds >> 1);

    return fat_time;
}

#endif /* SENSING1_USE_DATALOG */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
