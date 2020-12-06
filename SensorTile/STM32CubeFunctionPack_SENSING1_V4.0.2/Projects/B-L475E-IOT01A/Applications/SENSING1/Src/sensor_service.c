/**
  ******************************************************************************
  * @file    sensor_service.c
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Add 4 bluetooth services using vendor specific profiles.
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
#include "sensor_service.h"
#include "cli_commands.h"

#ifndef SENSING1_BlueNRG2
  #include "bluenrg_utils.h"
  #include "bluenrg_l2cap_aci.h"
#else /* SENSING1_BlueNRG2 */
  #include "bluenrg1_l2cap_aci.h"
#endif /* SENSING1_BlueNRG2 */

#include "uuid_ble_service.h"
#include "PowerControl.h"

#if SENSING1_USE_DATALOG
#include "DataLog_Manager.h"
#include <math.h>
#endif /* SENSING1_USE_DATALOG */

#include "OTA.h"

/* Exported variables ---------------------------------------------------------*/
uint8_t set_connectable = TRUE;

uint32_t ConnectionBleStatus  =0;

/* Imported Variables -------------------------------------------------------------*/

extern volatile float RMS_Ch[];
extern float DBNOISE_Value_Old_Ch[];

extern uint8_t bdaddr[6];

extern uint8_t NodeName[8];

#ifdef STM32_SENSORTILEBOX
extern volatile int PowerButtonPressed;
#endif /* STM32_SENSORTILEBOX */

extern  volatile int RebootBoard;


/* Private variables ------------------------------------------------------------*/
#ifndef USE_STM32L475E_IOT01
static uint32_t FeatureMask;
#endif /* USE_STM32L475E_IOT01 */
static uint16_t HWServW2STHandle;
static uint16_t EnvironmentalCharHandle;
static uint16_t AccGyroMagCharHandle;
#ifndef USE_STM32L475E_IOT01
static uint16_t AccEventCharHandle;
#endif /* USE_STM32L475E_IOT01 */
static uint16_t AudioLevelCharHandle;

#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
static uint16_t BatteryFeaturesCharHandle;
#endif /* ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX)) */

#if SENSING1_USE_DATALOG
  static uint16_t SDLogFeaturesCharHandle;
#endif /* SENSING1_USE_DATALOG */

static uint16_t ActivityRecCharHandle;
static uint16_t AudioSRecCharHandle;

static uint16_t ConfigServW2STHandle;
static uint16_t ConfigCharHandle;

static uint16_t ConsoleW2STHandle;
static uint16_t TermCharHandle;
static uint16_t StdErrCharHandle;

static uint8_t LastStderrBuffer[W2ST_MAX_CHAR_LEN];
static uint8_t LastStderrLen;
static uint8_t LastTermBuffer[W2ST_MAX_CHAR_LEN];
static uint8_t LastTermLen;

static uint8_t  EnvironmentalCharSize = 2; /* Size for Environmental BLE characteristic */
static uint16_t connection_handle = 0;
static uint32_t SizeOfUpdateBlueFW=0;

#ifdef SENSING1_BlueNRG2
  Service_UUID_t service_uuid;
  Char_UUID_t char_uuid;
#endif /* SENSING1_BlueNRG2 */

/* Private functions ------------------------------------------------------------*/
#ifndef SENSING1_BlueNRG2
static void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
static void GAP_DisconnectionComplete_CB(void);
#endif /* SENSING1_BlueNRG2 */
static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length);
static uint32_t ConfigCommandParsing(uint8_t * att_data, uint8_t data_length);

static void Read_Request_CB(uint16_t handle);
#ifndef SENSING1_BlueNRG2
static void HCI_LE_Event_CB(evt_le_meta_event *le_meta_evt);
static void HCI_Vendor_Event_CB(evt_blue_aci *blue_evt);
static void Attribute_Modified_CB(uint16_t attr_handle, uint8_t * att_data, uint8_t data_length);
#else /* SENSING1_BlueNRG2 */
static void Attribute_Modified_Request_CB(uint16_t Connection_Handle, uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data);
#endif /* SENSING1_BlueNRG2 */

/* Private define ------------------------------------------------------------*/

#ifdef ACC_BLUENRG_CONGESTION
#define ACI_GATT_UPDATE_CHAR_VALUE safe_aci_gatt_update_char_value
static int32_t breath;


/* @brief  Update the value of a characteristic avoiding (for a short time) to
 *         send the next updates if an error in the previous sending has
 *         occurred.
 * @param  servHandle The handle of the service
 * @param  charHandle The handle of the characteristic
 * @param  charValOffset The offset of the characteristic
 * @param  charValueLen The length of the characteristic
 * @param  charValue The pointer to the characteristic
 * @retval tBleStatus Status
 */
tBleStatus safe_aci_gatt_update_char_value(uint16_t servHandle,
                      uint16_t charHandle,
                      uint8_t charValOffset,
                      uint8_t charValueLen,
#ifndef SENSING1_BlueNRG2
                      const uint8_t *charValue)
#else /* SENSING1_BlueNRG2 */
                      uint8_t *charValue)
#endif /* SENSING1_BlueNRG2 */
{
  tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;

  if (breath > 0) {
    breath--;
  } else {
    ret = aci_gatt_update_char_value(servHandle,charHandle,charValOffset,charValueLen,charValue);

    if (ret != BLE_STATUS_SUCCESS){
      breath = ACC_BLUENRG_CONGESTION_SKIP;
    }
  }

  return (ret);
}

#else /* ACC_BLUENRG_CONGESTION */
#define ACI_GATT_UPDATE_CHAR_VALUE aci_gatt_update_char_value
#endif /* ACC_BLUENRG_CONGESTION */


/**
 * @brief  Add the Config service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_ConfigW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_CONFIG_SERVICE_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+3,&ConfigServW2STHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+3,&ConfigServW2STHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_CONFIG_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(ConfigServW2STHandle, UUID_TYPE_128, uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &ConfigCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(ConfigServW2STHandle, UUID_TYPE_128, &char_uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &ConfigCharHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  return BLE_STATUS_SUCCESS;

fail:
  return BLE_STATUS_ERROR;
}


/**
 * @brief  Add the Console service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_ConsoleW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_CONSOLE_SERVICE_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 1+3*2,&ConsoleW2STHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+3*2,&ConsoleW2STHandle);
#endif /* SENSING1_BlueNRG2 */
  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_TERM_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_WRITE | CHAR_PROP_READ ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &TermCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, &char_uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_WRITE | CHAR_PROP_READ ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &TermCharHandle);
#endif /* SENSING1_BlueNRG2 */  

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_STDERR_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &StdErrCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(ConsoleW2STHandle, UUID_TYPE_128, &char_uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &StdErrCharHandle);
#endif /* SENSING1_BlueNRG2 */ 

  if (ret != BLE_STATUS_SUCCESS) {
     goto fail;
  }

  return BLE_STATUS_SUCCESS;

fail:
  return BLE_STATUS_ERROR;
}


tBleStatus UpdateTermStdOut(uint8_t *data,uint8_t length)
{
  if (aci_gatt_update_char_value(ConsoleW2STHandle, TermCharHandle, 0, length , data) != BLE_STATUS_SUCCESS) {
      SENSING1_PRINTF("Error Updating Stdout Char\r\n");
    return BLE_STATUS_ERROR;
  }
  osDelay(20);
  return BLE_STATUS_SUCCESS;
}

tBleStatus UpdateTermStdErr(uint8_t *data,uint8_t length)
{
  if (aci_gatt_update_char_value(ConsoleW2STHandle, StdErrCharHandle, 0, length , data) != BLE_STATUS_SUCCESS) {
      SENSING1_PRINTF("Error Updating StdErr Char\r\n");
    return BLE_STATUS_ERROR;
  }
  osDelay(20);
  return BLE_STATUS_SUCCESS;
}


/**
 * @brief  Update Stderr characteristic value
 * @param  uint8_t *data string to write
 * @param  uint8_t length length of string to write
 * @retval tBleStatus      Status
 */
tBleStatus Stderr_Update(uint8_t *data, uint8_t length)
{
  uint8_t Offset;
  uint8_t DataToSend;
  msgData_t msg;
  msg.type        = TERM_STDERR;

  /* Split the code in packages*/
  for(Offset =0; Offset<length; Offset +=W2ST_MAX_CHAR_LEN){
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>W2ST_MAX_CHAR_LEN) ?  W2ST_MAX_CHAR_LEN : DataToSend;

    /* keep a copy */
    memcpy(LastStderrBuffer,data+Offset,DataToSend);
    LastStderrLen = DataToSend;

    msg.term.length   =  DataToSend;
    memcpy(msg.term.data,data+Offset,DataToSend);
    SendMsgToHost(&msg);

  }
  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Update Terminal characteristic value
 * @param  uint8_t *data string to write
 * @param  uint32_t length Length of string to write
 * @retval tBleStatus      Status
 */
tBleStatus Term_Update(uint8_t *data, uint8_t length)
{
  uint32_t  Offset;
  uint8_t   DataToSend;
  msgData_t msg;
  msg.type = TERM_STDOUT;

  /* Split the code in packages */
  for(Offset =0; Offset<length; Offset +=W2ST_MAX_CHAR_LEN){
    DataToSend = (length-Offset);
    DataToSend = (DataToSend>W2ST_MAX_CHAR_LEN) ?  W2ST_MAX_CHAR_LEN : DataToSend;

    /* keep a copy */
    memcpy(LastTermBuffer,data+Offset,DataToSend);
    LastTermLen = DataToSend;

    msg.term.length   =  DataToSend;
    memcpy(msg.term.data,data+Offset,DataToSend);
    SendMsgToHost(&msg);
  }
  return BLE_STATUS_SUCCESS;
}


/**
 * @brief  Update Stderr characteristic value after a read request
 * @param None
 * @retval tBleStatus      Status
 */
static tBleStatus Stderr_Update_AfterRead(void)
{
  msgData_t msg;
  msg.type        = TERM_STDERR;
  msg.term.length = LastStderrLen;
  memcpy(msg.term.data,LastStderrBuffer,LastStderrLen);
  SendMsgToHost(&msg);
  return BLE_STATUS_SUCCESS;
}


/**
 * @brief  Update Terminal characteristic value after a read request
 * @param None
 * @retval tBleStatus      Status
 */
static tBleStatus Term_Update_AfterRead(void)
{
  msgData_t msg;
  msg.type        = TERM_STDOUT;
  msg.term.length = LastTermLen;
  memcpy(msg.term.data,LastTermBuffer,LastTermLen);
  SendMsgToHost(&msg);
  return BLE_STATUS_SUCCESS;
}
/**
 * @brief  Update Activity Recognition value
 * @param  HAR_output_t ActivityCode Activity Recognized
 * @retval tBleStatus      Status
 */
tBleStatus ActivityRec_Update(HAR_output_t ActivityCode, HAR_algoIdx_t algoIdx)
{
  tBleStatus ret;
  uint8_t buff[2+1+1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = ActivityCode;
  
  if (HAR_IGN_WSDM_IDX == algoIdx) {
    buff[3] = HAR_IGN_WSDM_ALG_ID;
  } else if (HAR_IGN_IDX == algoIdx) {
    buff[3] = HAR_IGN_ALG_ID ;
  }else if (HAR_GMP_IDX == algoIdx) {
    buff[3] = HAR_GMP_ALG_ID;
  }

  ret = aci_gatt_update_char_value(HWServW2STHandle, ActivityRecCharHandle, 0, 2+1+1, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating ActivityRec Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating ActivityRec Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
/**
 * @brief  Update Audio Scene Recognition value
 * @param  ASC_OutputTypeDef SceneClassificationCode Scene Recognized
 * @retval tBleStatus      Status
 */
tBleStatus AudioSRec_Update(ASC_OutputTypeDef SceneClassificationCode)
{
  tBleStatus ret;

  uint8_t buff[2+ 1];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = SceneClassificationCode;

  ret = aci_gatt_update_char_value(HWServW2STHandle, AudioSRecCharHandle, 0, 2+1, buff);

  if (ret != BLE_STATUS_SUCCESS) {
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating AudioRec Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating AudioRec Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/* @brief  Send a notification for answering to a configuration command for Accelerometer events
 * @param  uint32_t Feature Feature calibrated
 * @param  uint8_t Command Replay to this Command
 * @param  uint8_t data result to send back
 * @retval tBleStatus Status
 */
tBleStatus Config_Notify(uint32_t Feature,uint8_t Command,uint8_t data)
{
  msgData_t msg;
  msg.type         = CONF_NOTIFY;
  msg.conf.feature = Feature;
  msg.conf.command = Command;
  msg.conf.data    = data;
  SendMsgToHost(&msg);
  return 0;  //!!!
}

tBleStatus Config_NotifyBLE(uint32_t Feature,uint8_t Command,uint8_t data)
{
  tBleStatus ret;
  uint8_t buff[2+4+1+1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  STORE_BE_32(buff+2,Feature);
  buff[6] = Command;
  buff[7] = data;

  ret = aci_gatt_update_char_value (ConfigServW2STHandle, ConfigCharHandle, 0, 8,buff);
  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Configuration Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating Configuration Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

#ifndef USE_STM32L475E_IOT01
/**
 * @brief  Send a notification When the DS3 detects one Acceleration event
 * @param  Command to Send
 * @retval tBleStatus Status
 */
tBleStatus AccEvent_Notify(uint16_t Command, uint8_t dimByte)
{
  tBleStatus ret= BLE_STATUS_SUCCESS;
  uint8_t buff[2+3];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));

  if(dimByte==3) {
    buff[2]= 0;
    STORE_LE_16(buff+3,Command);
  } else {
    STORE_LE_16(buff+2,Command);
  }

  ret = aci_gatt_update_char_value(HWServW2STHandle, AccEventCharHandle, 0, 2+dimByte,buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating AccEvent_Notify Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating AccEvent_Notify Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
#endif /* USE_STM32L475E_IOT01 */

/**
 * @brief  Add the HW Features service using a vendor specific profile
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_HW_SW_ServW2ST_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  /* default characteristics :
  1- HW_SENS_W2ST_SERVICE_UUID
  2- ENVIRONMENTAL_W2ST_CHAR_UUID
  3- ACC_GYRO_MAG_W2ST_CHAR_UUID
  4- ACC_EVENT_W2ST_CHAR_UUID
  5- MIC_W2ST_CHAR_UUID
  6- COPY_ACTIVITY_REC_W2ST_CHAR_UUID
  7- COPY_AUDIO_REC_W2ST_CHAR_UUID
 */
  uint8_t max_attr_records = 7;

#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
  if(TargetBoardFeatures.HandleGGComponent) {
    /* Battery Present */
    max_attr_records++;
  }
#endif /* ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX)) */

/* SD Log Feature */
#if SENSING1_USE_DATALOG
  max_attr_records++;
#endif /* SENSING1_USE_DATALOG */

  COPY_HW_SENS_W2ST_SERVICE_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE,
                          1+3*max_attr_records,
                          &HWServW2STHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128,  &service_uuid, PRIMARY_SERVICE, 1+3*max_attr_records,&HWServW2STHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  /* Fill the Environmental BLE Characteristc */
  COPY_ENVIRONMENTAL_W2ST_CHAR_UUID(uuid);
  if(TargetBoardFeatures.NumTempSensors==2) {
    uuid[14] |= 0x05; /* Two Temperature values*/
    EnvironmentalCharSize+=2*2;
  } else if(TargetBoardFeatures.NumTempSensors==1) {
    uuid[14] |= 0x04; /* One Temperature value*/
    EnvironmentalCharSize+=2;
  }
  
  if (TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    uuid[14] |= 0x08; /* Humidity */
    EnvironmentalCharSize+=2;
  }

  if (TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    uuid[14] |= 0x10; /* Pressure value*/
    EnvironmentalCharSize+=4;
  }

#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, EnvironmentalCharSize,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &EnvironmentalCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, EnvironmentalCharSize,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &EnvironmentalCharHandle);
#endif /* SENSING1_BlueNRG2 */  

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

  COPY_ACC_GYRO_MAG_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+3*3*2,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AccGyroMagCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+3*3*2,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AccGyroMagCharHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

#ifndef USE_STM32L475E_IOT01
  COPY_ACC_EVENT_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+3,
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &AccEventCharHandle);
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+3, 
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &AccEventCharHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }
#endif /* USE_STM32L475E_IOT01 */

  COPY_MIC_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid,2+AUDIO_CHANNELS,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AudioLevelCharHandle); 
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+AUDIO_CHANNELS,
                           CHAR_PROP_NOTIFY,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AudioLevelCharHandle);
#endif /* SENSING1_BlueNRG2 */
  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }
  

  COPY_ACTIVITY_REC_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+1+1, /* 2 byte timestamp, 1 byte action, 1 byte algorithm */
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &ActivityRecCharHandle);
  
#else /* SENSING1_BlueNRG2 */
  BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+1+1, /* 2 byte timestamp, 1 byte action, 1 byte algorithm */
                           CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &ActivityRecCharHandle);
#endif /* SENSING1_BlueNRG2 */

  if (ret != BLE_STATUS_SUCCESS) {
    goto fail;
  }

   COPY_AUDIO_REC_W2ST_CHAR_UUID(uuid);
 #ifndef SENSING1_BlueNRG2
   ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+1, /* 2 byte timestamp, 1 byte aucoustic scene classification */
                            CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                            ATTR_PERMISSION_NONE,
                            GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                            16, 0, &AudioSRecCharHandle);
 #else /* SENSING1_BlueNRG2 */
   BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
   ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid,  2+1, /* 2 byte timestamp, 1 byte aucoustic scene classification */
                            CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                            ATTR_PERMISSION_NONE,
                            GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                            16, 0, &AudioSRecCharHandle);
 #endif /* SENSING1_BlueNRG2 */
   if (ret != BLE_STATUS_SUCCESS) {
     goto fail;
   }

#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
  if(TargetBoardFeatures.HandleGGComponent) {
    COPY_GG_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
    ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, 2+2+2+2+1,
                             CHAR_PROP_NOTIFY,
                             ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                             16, 0, &BatteryFeaturesCharHandle);
#else /* SENSING1_BlueNRG2 */
    BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
    ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+2+2+2+1,
                             CHAR_PROP_NOTIFY,
                             ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                             16, 0, &BatteryFeaturesCharHandle);
#endif /* SENSING1_BlueNRG2 */
    if (ret != BLE_STATUS_SUCCESS) {
      goto fail;
    }
  }
#endif /* ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX)) */

#if SENSING1_USE_DATALOG
    COPY_SDLOG_W2ST_CHAR_UUID(uuid);
#ifndef SENSING1_BlueNRG2
    ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &SDLogFeaturesCharHandle);
#else /* SENSING1_BlueNRG2 */
    BLUENRG_memcpy(&char_uuid.Char_UUID_128, uuid, 16);
    ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, W2ST_MAX_CHAR_LEN,
                           CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 1, &SDLogFeaturesCharHandle);
#endif /* SENSING1_BlueNRG2 */
    if (ret != BLE_STATUS_SUCCESS) {
      goto fail;
    }
#endif /* SENSING1_USE_DATALOG */

  return BLE_STATUS_SUCCESS;

fail:
  return BLE_STATUS_ERROR;
}

/**
 * @brief  Update acceleration/Gryoscope and Magneto characteristics value
 * @param  MOTION_SENSOR_Axes_t Acc Structure containing acceleration value in mg
 * @param  MOTION_SENSOR_Axes_t Gyro Structure containing Gyroscope value
 * @param  MOTION_SENSOR_Axes_t Mag Structure containing magneto value
 * @retval tBleStatus      Status
 */
tBleStatus AccGyroMag_Update(MOTION_SENSOR_Axes_t *Acc,MOTION_SENSOR_Axes_t *Gyro,MOTION_SENSOR_Axes_t *Mag)
{
  tBleStatus ret;

  uint8_t buff[2+3*3*2];

  STORE_LE_16(buff   ,(HAL_GetTick()>>3));

  STORE_LE_16(buff+2 ,Acc->x);
  STORE_LE_16(buff+4 ,Acc->y);
  STORE_LE_16(buff+6 ,Acc->z);

  Gyro->x/=100;
  Gyro->y/=100;
  Gyro->z/=100;

  STORE_LE_16(buff+8 ,Gyro->x);
  STORE_LE_16(buff+10,Gyro->y);
  STORE_LE_16(buff+12,Gyro->z);

  STORE_LE_16(buff+14, Mag->x);
  STORE_LE_16(buff+16, Mag->y);
  STORE_LE_16(buff+18, Mag->z);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(HWServW2STHandle, AccGyroMagCharHandle, 0, 2+3*3*2, buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Acc/Gyro/Mag Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating Acc/Gyro/Mag Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}


/**
 * @brief  Update Environmental characteristic value
 * @param  int32_t Press Pressure in mbar
 * @param  uint16_t Hum humidity RH (Relative Humidity) in thenths of %
 * @param  int16_t Temp1 Temperature in tenths of degree first  sensor
 * @param  int16_t Temp2 Temperature in tenths of degree second sensor
 * @retval tBleStatus   Status
 */
tBleStatus Environmental_Update(int32_t Press,uint16_t Hum,int16_t Temp1,int16_t Temp2)
{
  tBleStatus ret;
  uint8_t BuffPos;

  uint8_t buff[2+4/*Press*/+2/*Hum*/+2/*Temp2*/+2/*Temp1*/];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  BuffPos=2;

  if (TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    STORE_LE_32(buff+BuffPos,Press);
    BuffPos+=4;
  }

  if (TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    STORE_LE_16(buff+BuffPos,Hum);
    BuffPos+=2;
  }

  if (TargetBoardFeatures.HandleTempSensors[0] != SENSING1_SNS_NOT_VALID) {
    STORE_LE_16(buff+BuffPos,Temp1);
    BuffPos+=2;
  }

  if (TargetBoardFeatures.HandleTempSensors[1] != SENSING1_SNS_NOT_VALID) {
    STORE_LE_16(buff+BuffPos,Temp2);
    BuffPos+=2;
  }

  ret = aci_gatt_update_char_value(HWServW2STHandle, EnvironmentalCharHandle, 0, EnvironmentalCharSize,buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Error Updating Environmental Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating Environmental Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Update Microphones characteristic values
 * @param  uint16_t *Mic SNR dB Microphones array
 * @retval tBleStatus   Status
 */
tBleStatus AudioLevel_Update(uint16_t *Mic)
{
  tBleStatus ret;
  uint16_t Counter;

  uint8_t buff[2+1*AUDIO_CHANNELS]; /* BlueCoin has 4 Mics */

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  for(Counter=0;Counter<AUDIO_CHANNELS;Counter++) {
    buff[2+Counter]= Mic[Counter]&0xFF;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(HWServW2STHandle, AudioLevelCharHandle, 0, 2+AUDIO_CHANNELS,buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite = sprintf((char *)BufferToWrite, "Error Updating Mic Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
/**
 * @brief  Update Gas Gouge characteristic
 * @param  None
 * @retval tBleStatus   Status
 */
tBleStatus GG_Update(uint32_t soc, uint32_t voltage, int32_t current)
{  
  tBleStatus ret;

  uint8_t buff[2+2+2+2+1];

#ifdef STM32_SENSORTILE
  /* Current in mA */
  current=current/10;
#endif /* STM32_SENSORTILE */

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  STORE_LE_16(buff+2,soc*10);
  STORE_LE_16(buff+4,voltage);
  STORE_LE_16(buff+6,current);

#ifdef STM32_SENSORTILE
  if(soc<15) {
    /* if it's < 15% Low Battery*/
    buff[8] = 0x00; /* Low Battery */
  } else {
    static uint32_t Status     = 0x04; /* Unknown */
    if(current <= 0) {
      Status = 0x01; /* Discharging */
    } else {
      Status = 0x03; /* Charging */
    }
    buff[8] = Status;
  }
#elif STM32_SENSORTILEBOX
   /* The Charge Status is unknow because we could not use the the Interrupt
     * for understanding the status change.
     * So it's possible to see the status only measuring the Voltage change */
   buff[8]     = 0x04; /* Unknown */
#endif /* STM32_SENSORTILE */ 

  ret = aci_gatt_update_char_value(HWServW2STHandle, BatteryFeaturesCharHandle, 0, 2+2+2+2+1,buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite = sprintf((char *)BufferToWrite, "Error Updating GG Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating GG Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
#endif /* STM32_SENSORTILE */

#if SENSING1_USE_DATALOG
/**
 * @brief  Update SD Log characteristic
 * @param  None
 * @retval tBleStatus   Status
 */
tBleStatus SDLog_Update(uint8_t ErrorCode)
{
  tBleStatus ret;
  uint8_t buff[2+1];

  STORE_LE_16(buff  ,(HAL_GetTick()>>3));
  buff[2] = ErrorCode;

  ret = aci_gatt_update_char_value(HWServW2STHandle, SDLogFeaturesCharHandle, 0, 3,buff);

  if (ret != BLE_STATUS_SUCCESS){
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite = sprintf((char *)BufferToWrite, "Error Updating SD Log Char\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Error Updating SD Log Char\r\n");
    }
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}
#endif /* SENSING1_USE_DATALOG */

/**
 * @brief  Puts the device in connectable mode.
 * @param  None
 * @retval None
 */
void setConnectable(void)
{
#ifndef SENSING1_BlueNRG2
  char local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,NodeName[1],NodeName[2],NodeName[3],NodeName[4],NodeName[5],NodeName[6],NodeName[7]};
#else /* SENSING1_BlueNRG2 */
  uint8_t local_name[8] = {AD_TYPE_COMPLETE_LOCAL_NAME,NodeName[1],NodeName[2],NodeName[3],NodeName[4],NodeName[5],NodeName[6],NodeName[7]};
#endif /* SENSING1_BlueNRG2 */

  uint8_t manuf_data[26] = {
    2,0x0A,0x00, /* 0 dBm Transmission Power */
    8,0x09,NodeName[1],NodeName[2],NodeName[3],NodeName[4],NodeName[5],NodeName[6],NodeName[7], // Complete Name
    13,0xFF,0x01/*SKD version */,
#if defined(USE_STM32L4XX_NUCLEO)
    0x80,
#elif defined(STM32_SENSORTILE)
    0x02,
#elif defined(USE_STM32L475E_IOT01)
    0x07,
#elif defined(STM32_SENSORTILEBOX)
    0x06,
#else /* USE_STM32L4XX_NUCLEO */
#error "Define the right platform"    
#endif /* USE_STM32L4XX_NUCLEO */
    0x00 /* AudioSync+AudioData */,
    0xE0 /* ACC+Gyro+Mag*/,
    0x00 /*  */,
    0x00, /*  */
    0x00, /* BLE MAC start */
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* BLE MAC stop */
  };

  /* BLE MAC */
  manuf_data[20] = bdaddr[5];
  manuf_data[21] = bdaddr[4];
  manuf_data[22] = bdaddr[3];
  manuf_data[23] = bdaddr[2];
  manuf_data[24] = bdaddr[1];
  manuf_data[25] = bdaddr[0];

#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
  if(TargetBoardFeatures.HandleGGComponent){
    manuf_data[17] |= 0x02; /* Battery Present */
  }
#endif /* ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX)) */

  manuf_data[16] |= 0x04; /* Mic */

  if(TargetBoardFeatures.NumTempSensors==2) {
    manuf_data[17] |= 0x05; /* Two Temperature values*/
  } else if(TargetBoardFeatures.NumTempSensors==1) {
    manuf_data[17] |= 0x04; /* One Temperature value*/
  }

  if (TargetBoardFeatures.HandleHumSensor != SENSING1_SNS_NOT_VALID) {
    manuf_data[17] |= 0x08; /* Humidity */
  }

  if (TargetBoardFeatures.HandlePressSensor != SENSING1_SNS_NOT_VALID) {
    manuf_data[17] |= 0x10; /* Pressure value*/
  }

#ifndef USE_STM32L475E_IOT01
  /* Accelerometer Events */
  manuf_data[18] |=0x04;
#endif /* USE_STM32L475E_IOT01 */

    manuf_data[19] |= 0x10;

  /* disable scan response */
#ifndef SENSING1_BlueNRG2
  hci_le_set_scan_resp_data(0,NULL);
#else /* SENSING1_BlueNRG2 */
  hci_le_set_scan_response_data(0,NULL);
#endif /* SENSING1_BlueNRG2 */

#ifndef BLE_CHANGE_ADV_INTERVAL
  aci_gap_set_discoverable(ADV_IND, 0, 0,
#else /* BLE_CHANGE_ADV_INTERVAL */
  aci_gap_set_discoverable(ADV_IND, 0x0640, 0x0640,
/* 0x800 default value - 1.28 s
 0x640  - 1.00 s */
#endif /* BLE_CHANGE_ADV_INTERVAL */
#ifndef MAC_SENSING1
  #ifdef MAC_STM32UID_SENSING1
                           STATIC_RANDOM_ADDR,
  #else /* MAC_STM32UID_SENSING1 */
                           RANDOM_ADDR,
  #endif /* MAC_STM32UID_SENSING1 */
#else /* MAC_SENSING1 */
                           PUBLIC_ADDR,
#endif /* MAC_SENSING1 */
                           NO_WHITE_LIST_USE,
                           sizeof(local_name), local_name, 0, NULL, 0, 0);

  /* Send Advertising data */
  aci_gap_update_adv_data(26, manuf_data);
}
/**
 * @brief  Exits the device from connectable mode.
 * @param  None
 * @retval None
 */
void setNotConnectable(void)
{
  aci_gap_set_non_discoverable();
}
#ifdef BLE_LINK_ADAPT
void setConnectionParameters(int min , int max, int latency , int timeout )
{
  int ret = aci_l2cap_connection_parameter_update_request(connection_handle,
                                              min /* interval_min*/,
                                              max /* interval_max */,
                                              latency /* slave_latency */,
                                              timeout /*timeout_multiplier*/);
  if (ret != BLE_STATUS_SUCCESS) {
    while (1) {
      ;
    }
  }
}
#endif

#ifndef STM32_SENSORTILEBOX
/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t addr[6] Address of peer device
 * @param  uint16_t handle Connection handle
 * @retval None
 */
static void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{
  connection_handle = handle;

  SENSING1_PRINTF(">>>>>>CONNECTED %02x:%02x:%02x:%02x:%02x:%02x\r\n", addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);

  ConnectionBleStatus=0;
#ifndef USE_STM32L475E_IOT01
  DisableHWFeatures();
#endif /* USE_STM32L475E_IOT01 */
  LedBlinkStop();
}

/**
 * @brief  This function is called when the peer device get disconnected.
 * @param  None
 * @retval None
 */
static void GAP_DisconnectionComplete_CB(void)
{
  SENSING1_PRINTF("<<<<<<DISCONNECTED\r\n");

  /* Make the device connectable again. */
  set_connectable = TRUE;
  ConnectionBleStatus=0;

#ifndef USE_STM32L475E_IOT01
  DisableHWFeatures();
#endif /* USE_STM32L475E_IOT01 */

  /* Reset for any problem during FOTA update */
  SizeOfUpdateBlueFW = 0;

#if SENSING1_USE_DATALOG
  /* Close the Log if the user exit in a dirty way */

  if(SD_LogMems_Enabled) {
    SD_Card_FeaturesMask &= ~(FEATURE_MASK_TEMP1 |
                              FEATURE_MASK_TEMP2|
                              FEATURE_MASK_PRESS |
                              FEATURE_MASK_HUM |
                              FEATURE_MASK_ACC |
                              FEATURE_MASK_GRYO |
                              FEATURE_MASK_MAG);
    SD_CardLoggingMemsStop();
  }

  if(SD_LogAudio_Enabled) {
    /* For waiting the close of the MEMS/Annotation file */
    osDelay(100);

    SD_Card_FeaturesMask &= ~FEATURE_MASK_BLUEVOICE;
    SD_CardLoggingAudioStop();
  }
#endif /* SENSING1_USE_DATALOG */

  /************************/
  /* Stops all the Timers */
  /************************/
  /* Stop Timer For MotionAR */
  switch(HarAlgo) {
    case HAR_GMP_IDX:
      stopProc(ACTIVITY_GMP);
    break;
    case HAR_IGN_IDX:
      stopProc(ACTIVITY_IGN);
    break;
    case HAR_IGN_WSDM_IDX:
      stopProc(ACTIVITY_IGN_WSDM);
    break;
    default:
    	break;
  }

  /* Stop Timer For Acc/Gyro/Mag */
  stopProc(MOTION);

  /* Stop Timer For Environmental */
  stopProc(ENV);

  /* Stop Timer For Audio Level*/
  stopProc(AUDIO_LEV);
}
#endif /* STM32_SENSORTILEBOX */

/**
 * @brief  This function is called when there is a Bluetooth Read request
 * @param  uint16_t handle Handle of the attribute
 * @retval None
 */
void Read_Request_CB(uint16_t handle)
{
  if(handle == EnvironmentalCharHandle + 1){
    /* Read Request for Pressure,Humidity, and Temperatures*/
    int32_t PressToSend;
    uint16_t HumToSend;
    int16_t Temp2ToSend,Temp1ToSend;

    /* Read all the Environmental Sensors */
    ReadEnvironmentalData(&PressToSend,&HumToSend, &Temp1ToSend,&Temp2ToSend);

    /* Send the Data with BLE */
    Environmental_Update(PressToSend,HumToSend,Temp1ToSend,Temp2ToSend);
#ifndef USE_STM32L475E_IOT01
  } else if(handle == AccEventCharHandle +1) {
    {
      uint16_t StepCount;
      if(W2ST_CHECK_HW_FEATURE(W2ST_HWF_PEDOMETER)) {
        StepCount = GetStepHWPedometer();
      } else {
        StepCount = 0;
      }
      AccEvent_Notify(StepCount, 2);
    }
#endif /* USE_STM32L475E_IOT01 */
  } else if (handle == StdErrCharHandle + 1) {
    /* Send again the last packet for StdError */
    Stderr_Update_AfterRead();
  } else if (handle == TermCharHandle + 1) {
    /* Send again the last packet for Terminal */
    Term_Update_AfterRead();
  } else if (handle == ActivityRecCharHandle + 1) {
     ActivityRec_Update(HAR_get_Activity_Code(HarAlgo), HarAlgo);
  } else if (handle == AudioSRecCharHandle + 1) {
    AudioSRec_Update(ASC_GetClassificationCode());
  }

  if(connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}
#ifndef SENSING1_BlueNRG2
/**
 * @brief  This function is called when there is a change on the gatt attribute
 * With this function it's possible to understand if one application
 * is subscribed or not to the one service
 * @param uint16_t att_handle Handle of the attribute
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval None
 */
static void Attribute_Modified_CB(uint16_t attr_handle, uint8_t * att_data, uint8_t data_length)
#else /* SENSING1_BlueNRG2 */
static void Attribute_Modified_Request_CB(uint16_t Connection_Handle, uint16_t attr_handle, uint16_t Offset, uint8_t data_length, uint8_t *att_data)
#endif /* SENSING1_BlueNRG2 */
{
  /* TODO: Remove delay */
  if(SizeOfUpdateBlueFW==0) {
    /* Not During FOTA */
    osDelay(100);
  }  
  
  if (attr_handle == ActivityRecCharHandle + 2) {
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_AR);
      switch(HarAlgo) {
        case HAR_GMP_IDX:
        case HAR_ALGO_IDX_NONE:
          HarAlgo = HAR_GMP_IDX;
          startProc(ACTIVITY_GMP,INERTIAL_ACQ_ACTIVITY_GMP_MS);
          SENSING1_PRINTF("HAR GMP started\r\n");
        break;
        case HAR_IGN_IDX:
          startProc(ACTIVITY_IGN,INERTIAL_ACQ_ACTIVITY_IGN_MS);
          SENSING1_PRINTF("HAR IGN started\r\n");
        break;
        case HAR_IGN_WSDM_IDX:
          startProc(ACTIVITY_IGN_WSDM,INERTIAL_ACQ_ACTIVITY_IGN_WSDM_MS);
          SENSING1_PRINTF("HAR IGN_WSDM started\r\n");
          break;
        default:
        break;
      }
    } else if (att_data[0] == 0) {
      W2ST_OFF_CONNECTION(W2ST_CONNECT_AR);
      switch(HarAlgo) {
        case HAR_GMP_IDX:
          stopProc(ACTIVITY_GMP);
          SENSING1_PRINTF("HAR GMP halted\r\n");
        break;
        case HAR_IGN_IDX:
          stopProc(ACTIVITY_IGN);
          SENSING1_PRINTF("HAR IGN halted\r\n");
        break;
        case HAR_IGN_WSDM_IDX:
          stopProc(ACTIVITY_IGN_WSDM);
          SENSING1_PRINTF("HAR IGN_WSDM halted\r\n");
          break;
		default:
		break;
      }
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"--->ActRec=%s\n",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_AR) ? " ON" : " OFF");
      Term_Update(BufferToWrite,BytesToWrite);
    } else
      SENSING1_PRINTF("--->ActRec=%s\r\n",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_AR) ? " ON" : " OFF");
#endif /* SENSING1_DEBUG_CONNECTION */
  }
  else
  if (attr_handle == AudioSRecCharHandle + 2) {
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_ASC_EVENT);
      startProc(AUDIO_SC, 0 /* Not Used for Audio */);
    } else if (att_data[0] == 0) {
      W2ST_OFF_CONNECTION(W2ST_CONNECT_ASC_EVENT);
      stopProc(AUDIO_SC);
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"--->AudioSRec=%s\n",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_ASC_EVENT) ? " ON" : " OFF");
      Term_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("--->AudioSRec=%s\r\n",  W2ST_CHECK_CONNECTION(W2ST_CONNECT_ASC_EVENT) ? " ON" : " OFF");
    }
#endif /* SENSING1_DEBUG_CONNECTION */
#if ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX))
  } else if(attr_handle == BatteryFeaturesCharHandle + 2){
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_GG_EVENT);
#ifdef STM32_SENSORTILEBOX
      BSP_BC_CmdSend(BATMS_ON);
#endif /* STM32_SENSORTILEBOX */
      startProc(BATTERY_INFO,ENV_UPDATE_MS);
    }else if (att_data[0] == 0){
      W2ST_OFF_CONNECTION(W2ST_CONNECT_GG_EVENT);
#ifdef STM32_SENSORTILEBOX
      BSP_BC_CmdSend(BATMS_OFF);
#endif /* STM32_SENSORTILEBOX */
      stopProc(BATTERY_INFO);
   }
#ifdef SENSING1_DEBUG_CONNECTION
   if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
     BytesToWrite =sprintf((char *)BufferToWrite,"--->GG=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_GG_EVENT) ? " ON" : " OFF");
     Term_Update(BufferToWrite,BytesToWrite);
   } else {
     SENSING1_PRINTF("--->GG=%s", W2ST_CHECK_CONNECTION(W2ST_CONNECT_GG_EVENT) ? " ON\r\n" : " OFF\r\n");
   }
#endif /* SENSING1_DEBUG_CONNECTION */
#endif /* ((defined STM32_SENSORTILE) | (defined STM32_SENSORTILEBOX)) */
   } else if(attr_handle == ConfigCharHandle + 2){
      if (att_data[0] == 01) {
        W2ST_ON_CONNECTION(W2ST_CONNECT_CONF_EVENT);
      } else if (att_data[0] == 0){
        W2ST_OFF_CONNECTION(W2ST_CONNECT_CONF_EVENT);
      }
#ifdef SENSING1_DEBUG_CONNECTION
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
        BytesToWrite =sprintf((char *)BufferToWrite,"--->Conf=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT) ? "ON" : "OFF");
       Term_Update(BufferToWrite,BytesToWrite);
      } else {
        SENSING1_PRINTF("--->Conf=%s\r\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT) ? "ON" : "OFF");
      }
#endif /* SENSING1_DEBUG_CONNECTION */
#if SENSING1_USE_DATALOG
    } else if(attr_handle == SDLogFeaturesCharHandle + 2){
      if (att_data[0] == 01) {
        W2ST_ON_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING);
      } else if (att_data[0] == 0){
        W2ST_OFF_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING);
      }
#ifdef SENSING1_DEBUG_CONNECTION
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
        BytesToWrite =sprintf((char *)BufferToWrite,"--->SDLog=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING) ? "ON" : "OFF");
       Term_Update(BufferToWrite,BytesToWrite);
      } else {
        SENSING1_PRINTF("--->SDLog=%s\r\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING) ? "ON" : "OFF");
      }
#endif /* SENSING1_DEBUG_CONNECTION */
#endif /* SENSING1_USE_DATALOG */
  } else if(attr_handle == AccGyroMagCharHandle + 2) {
     if (att_data[0] == 01) {
       W2ST_ON_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG);
       startProc(MOTION, INERTIAL_UPDATE_MS );
    } else if (att_data[0] == 0) {
       W2ST_OFF_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG);
       stopProc(MOTION);
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"--->Acc/Gyro/Mag=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG) ? " ON" : " OFF");
      Term_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("--->Acc/Gyro/Mag=%s\r\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_GYRO_MAG) ? " ON" : " OFF");
    }
#endif /* SENSING1_DEBUG_CONNECTION */
#ifndef USE_STM32L475E_IOT01
  } else if(attr_handle == AccEventCharHandle + 2) {
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_ACC_EVENT);
#ifndef STM32_SENSORTILEBOX
      EnableHWMultipleEvents();
      ResetHWPedometer();
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
        Config_Notify(FEATURE_MASK_ACC_EVENTS,'m',1);
      }
#else /* STM32_SENSORTILEBOX */
      EnableHWOrientation6D();
      if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
        Config_Notify(FEATURE_MASK_ACC_EVENTS,'o',1);
      }
#endif /* STM32_SENSORTILEBOX */
    } else if (att_data[0] == 0) {
      W2ST_OFF_CONNECTION(W2ST_CONNECT_ACC_EVENT);
#ifndef STM32_SENSORTILEBOX
      DisableHWMultipleEvents();
#else /* STM32_SENSORTILEBOX */
      DisableHWFeatures();
#endif /* STM32_SENSORTILEBOX */      
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"--->AccEvent=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_EVENT) ? " ON" : " OFF");
     Term_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("--->AccEvent=%s\r\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ACC_EVENT) ? " ON" : " OFF");
    }
#endif /* SENSING1_DEBUG_CONNECTION */
#endif /* USE_STM32L475E_IOT01 */
  } else if(attr_handle == EnvironmentalCharHandle + 2){
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_ENV);

      startProc(ENV,ENV_UPDATE_MS);
    } else if (att_data[0] == 0){
      W2ST_OFF_CONNECTION(W2ST_CONNECT_ENV);

      stopProc(ENV);
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"--->Env=%s\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ENV) ? " ON" : " OFF");
     Term_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("--->Env=%s\r\n", W2ST_CHECK_CONNECTION(W2ST_CONNECT_ENV) ? " ON" : " OFF");
    }
#endif /* SENSING1_DEBUG_CONNECTION */
  } else if(attr_handle == StdErrCharHandle + 2){
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_STD_ERR);
    } else if (att_data[0] == 0){
      W2ST_OFF_CONNECTION(W2ST_CONNECT_STD_ERR);
    }
  } else if(attr_handle == TermCharHandle + 2){
    if (att_data[0] == 01) {
      W2ST_ON_CONNECTION(W2ST_CONNECT_STD_TERM);
    } else if (att_data[0] == 0){
      W2ST_OFF_CONNECTION(W2ST_CONNECT_STD_TERM);
    }
  } else if (attr_handle == TermCharHandle + 1){
    uint32_t SendBackData =1; /* By default Answer with the same message received */
    if(SizeOfUpdateBlueFW!=0) {
      /* FP-AI-SENSING1 firwmare update */
      int8_t RetValue = UpdateFWBlueMS(&SizeOfUpdateBlueFW,att_data, data_length,1);
      if(RetValue!=0) {
        MCR_FAST_TERM_UPDATE_FOR_OTA(((uint8_t *)&RetValue));
        if(RetValue==1) {
          /* if OTA checked */
          SENSING1_PRINTF("%s will restart\r\n",SENSING1_PACKAGENAME);
          RebootBoard = 1;
        }
      }
      SendBackData=0;
    } else {
      /* Received one write from Client on Terminal characteristc */
      SendBackData = DebugConsoleCommandParsing(att_data,data_length);
    }

    /* Send it back if it's not recognized */
    if(SendBackData) {
      Term_Update(att_data,data_length);
    }
  } else if (attr_handle == AudioLevelCharHandle + 2) {
    if (att_data[0] == 01) {
      int32_t Count;

      W2ST_ON_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL);

      InitMics(AUDIO_SAMPLING_FREQUENCY);

      for(Count=0;Count<TargetBoardFeatures.NumMicSensors;Count++) {
        RMS_Ch[Count]=0;
        DBNOISE_Value_Old_Ch[Count] =0;
      }
      startProc(AUDIO_LEV,MICS_DB_UPDATE_MS);

    } else if (att_data[0] == 0) {
      W2ST_OFF_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL);

      DeInitMics();
      stopProc(AUDIO_LEV);
    }
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite = sprintf((char *)BufferToWrite,"--->dB Noise AudioLevel=%s\n", (W2ST_CHECK_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL)   ? " ON" : " OFF") );
      Term_Update(BufferToWrite,BytesToWrite);
    }else {
      SENSING1_PRINTF("--->dB Noise AudioLevel=%s\r\n", (W2ST_CHECK_CONNECTION(W2ST_CONNECT_AUDIO_LEVEL)   ? " ON" : " OFF"));
    }
#endif /* SENSING1_DEBUG_CONNECTION */
  } else if (attr_handle == ConfigCharHandle + 1) {
    /* Received one write command from Client on Configuration characteristc */
    ConfigCommandParsing(att_data, data_length);
#if SENSING1_USE_DATALOG
  } else if (attr_handle == SDLogFeaturesCharHandle + 1) {
    /* Received one write command from Client on SD Log characteristc */
    switch(att_data[0]) {
      case SD_CARD_LOGGING_STOP:
        /* Stop Log Features */
        if(SD_Card_FeaturesMask & (FEATURE_MASK_TEMP1 |
                                   FEATURE_MASK_TEMP2|
                                   FEATURE_MASK_PRESS |
                                   FEATURE_MASK_HUM |
                                   FEATURE_MASK_ACC |
                                   FEATURE_MASK_GRYO |
                                   FEATURE_MASK_MAG)) {

          SD_Card_FeaturesMask &= ~(FEATURE_MASK_TEMP1 |
                                   FEATURE_MASK_TEMP2|
                                   FEATURE_MASK_PRESS |
                                   FEATURE_MASK_HUM |
                                   FEATURE_MASK_ACC |
                                   FEATURE_MASK_GRYO |
                                   FEATURE_MASK_MAG);
          SD_CardLoggingMemsStop();
       } else if(SD_Card_FeaturesMask == FEATURE_MASK_BLUEVOICE) {
          SD_CardLoggingMemsStop();
       }

        if(SD_Card_FeaturesMask & FEATURE_MASK_BLUEVOICE) {
          /* For waiting the close of the MEMS/Annotation file */
          osDelay(100);

          SD_Card_FeaturesMask &= ~FEATURE_MASK_BLUEVOICE;
          SD_CardLoggingAudioStop();
        }

        /* Send back the message */
        if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
          SDLog_Update(SD_CARD_LOGGING_STOP);
        }
      break;
      case SD_CARD_LOGGING_START:
        /* Start Log Features */
#ifdef STM32_SENSORTILEBOX
        if(NoSDFlag) {
          DATALOG_SD_Init();
          osDelay(200);
        }

        if(NoSDFlag) {
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
            SDLog_Update(SD_CARD_LOGGING_NO_SD);
          }
          break;
        }
#endif /* STM32_SENSORTILEBOX */

        /* Read the Features Mask */
        {
          uint8_t *p8_Feature = (uint8_t *) &SD_Card_FeaturesMask;
          p8_Feature[0] = att_data[1];
          p8_Feature[1] = att_data[2];
          p8_Feature[2] = att_data[3];
          p8_Feature[3] = att_data[4];
#if 0
          /*Just for Debug */
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
            BytesToWrite =sprintf((char *)BufferToWrite, "FeatureMask=%X\n",SD_Card_FeaturesMask);
            Term_Update(BufferToWrite,BytesToWrite);
          }
#endif
        }

        /* Read the Sample rate for Inertial/Environmental Features*/
        {
          uint16_t SampleRateEnvFeatures;
          uint8_t *p8_SampleRateIneFeatures = (uint8_t *) &SampleRateIneFeatures;
          uint8_t *p8_SampleRateEnvFeatures = (uint8_t *) &SampleRateEnvFeatures;

          p8_SampleRateEnvFeatures[0] = att_data[5];
          p8_SampleRateEnvFeatures[1] = att_data[6];

          p8_SampleRateIneFeatures[0] = att_data[7];
          p8_SampleRateIneFeatures[1] = att_data[8];

          /* The Desired Sample Rate is Hz*10 */
          /* We have the thread for Saving the data that could be waked up every multiple of mSec */
          RoundedInertialWakeUpTimer = (int32_t) round(1000.0/(SampleRateIneFeatures/10));

          RoundCounterEnvironmental  = (int32_t) round((10000.0/RoundedInertialWakeUpTimer)/SampleRateEnvFeatures);
          RoundedEnvironmentalFreq   = (int32_t) round(10000.0/(RoundCounterEnvironmental*RoundedInertialWakeUpTimer));
        }

        /* Read the Microphone Volume Level */
        {
          int32_t AudioVolume = *((int8_t*)(att_data+9));

          if((AudioVolume<0) & (AudioVolume>64)) {
            BytesToWrite =sprintf((char *)BufferToWrite,"setMicVol Not Correct\n");
            Stderr_Update(BufferToWrite,BytesToWrite);
          } else {
            TargetBoardFeatures.AudioVolume = AudioVolume;
            BytesToWrite =sprintf((char *)BufferToWrite,"setMicVol Correct =%ld\n",
                                TargetBoardFeatures.AudioVolume);
            Term_Update(BufferToWrite,BytesToWrite);
          }
        }

        /* Read the Data File Name if it's present */
        if(data_length>10) {
          uint8_t Count;
          for(Count=0;Count<(data_length-10);Count++) {
            DefaultDataFileName[Count]=att_data[Count+9];
          }
          /* Termination String */
          DefaultDataFileName[Count] ='\0';
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
            BytesToWrite =sprintf((char *)BufferToWrite, "FileName %s\n",DefaultDataFileName);
            Term_Update(BufferToWrite,BytesToWrite);
          }
        }

        /* Start Log Inertial/Enviromental Feature */
        if(((SD_Card_FeaturesMask&FEATURE_MASK_TEMP1 )!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_TEMP2)!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_PRESS)!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_HUM  )!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_ACC  )!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_GRYO )!=0) |
           ((SD_Card_FeaturesMask&FEATURE_MASK_MAG  )!=0)) {
          SD_CardLoggingMemsStart(0);
         } else if (SD_Card_FeaturesMask==FEATURE_MASK_BLUEVOICE) {
           SD_CardLoggingMemsStart(1);
         }

        /* Start Log Inertial/Enviromental Feature */
        if(SD_Card_FeaturesMask&FEATURE_MASK_BLUEVOICE) {
          osDelay(100);
          SD_CardLoggingAudioStart();
        }

        /* Send back the message */
        if(SD_LogAudio_Enabled | SD_LogMems_Enabled) {
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_SD_CARD_LOGGING)) {
            SDLog_Update(SD_CARD_LOGGING_START);
          }
        }
      break;
      case SD_CARD_LOGGING_UPDATE:
        /* Update Annotation */
        SaveDataAnnotation(att_data+1);
        if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
          BytesToWrite =sprintf((char *)BufferToWrite, "Ann->[%s]\n",att_data+1);
          Term_Update(BufferToWrite,BytesToWrite);
        }
      break;
      default:
        SENSING1_PRINTF("SD Log Feature Error First Byte=%d\n",att_data[0]);
    }
#endif /* SENSING1_USE_DATALOG */
#ifdef BLE_FORCE_RESCAN
  } else if (attr_handle==(0x0002+2)) {
    /* Force one UUID rescan for FOTA */
    tBleStatus ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
    uint8_t buff[4];

    /* Delete all the Handles from 0x0001 to 0xFFFF */
    STORE_LE_16(buff  ,0x0001);
    STORE_LE_16(buff+2,0xFFFF);

    ret = aci_gatt_update_char_value(0x0001,0x0002,0,4,buff);

    if (ret == BLE_STATUS_SUCCESS){
      SENSING1_PRINTF("UUID Rescan Forced\r\n");
    } else {
      SENSING1_PRINTF("Problem forcing UUID Rescan\r\n");
    }
#endif /* BLE_FORCE_RESCAN */
  }else {   
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_ERR)){
      BytesToWrite =sprintf((char *)BufferToWrite, "Notification UNKNOWN handle\n");
      Stderr_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Notification UNKNOWN handle =%d\r\n",attr_handle);
    }
  }
}

static uint32_t DebugConsoleCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  BaseType_t xMoreDataToFollow;

  /* TODO: Check concurrency between UART and BLE CLIProcessCommand */

  /* Handle ST BLE Sensor smartphone app commands first */
  if (!strncmp("upgradeFw", (char *)att_data, 9))
  {
    uint32_t uwCRCValue;
    uint8_t *PointerByte = (uint8_t *)&SizeOfUpdateBlueFW;

    SizeOfUpdateBlueFW = atoi((char *)(att_data + 9));
    PointerByte[0] = att_data[9];
    PointerByte[1] = att_data[10];
    PointerByte[2] = att_data[11];
    PointerByte[3] = att_data[12];

    /* Check the Maximum Possible OTA size */
    if (SizeOfUpdateBlueFW > OTA_MAX_PROG_SIZE)
    {
      SENSING1_PRINTF("OTA %s SIZE=%ld > %d Max Allowed\r\n",
                      SENSING1_PACKAGENAME, SizeOfUpdateBlueFW, OTA_MAX_PROG_SIZE);
      /* Answer with a wrong CRC value for signaling the problem to BlueMS application */
      PointerByte[0] = att_data[13];
      PointerByte[1] = (att_data[14] != 0) ? 0 : 1; /* In order to be sure to have a wrong CRC */
      PointerByte[2] = att_data[15];
      PointerByte[3] = att_data[16];
      BytesToWrite = 4;
      Term_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      PointerByte = (uint8_t *)&uwCRCValue;
      PointerByte[0] = att_data[13];
      PointerByte[1] = att_data[14];
      PointerByte[2] = att_data[15];
      PointerByte[3] = att_data[16];

      SENSING1_PRINTF("OTA %s SIZE=%ld uwCRCValue=%lx\r\n",
                      SENSING1_PACKAGENAME, SizeOfUpdateBlueFW, uwCRCValue);

      /* Reset the Flash */
      StartUpdateFWBlueMS(SizeOfUpdateBlueFW, uwCRCValue);

      /* Reduce the connection interval */
      {
#ifndef SENSING1_BlueNRG2
           int ret = aci_l2cap_connection_parameter_update_request(
#else /* SENSING1_BlueNRG2 */
           int ret = aci_l2cap_connection_parameter_update_req(
#endif /* SENSING1_BlueNRG2 */

                                                        connection_handle,
                                                        10 /* interval_min*/,
                                                        10 /* interval_max */,
                                                        0   /* slave_latency */,
                                                        400 /*timeout_multiplier*/);
          /* Go to infinite loop if there is one error */
          if (ret != BLE_STATUS_SUCCESS) {
            while (1) {
              SENSING1_PRINTF("Problem Changing the connection interval\r\n");
            }
          }
        }

        /* Signal that we are ready sending back the CRV value*/
        BufferToWrite[0] = PointerByte[0];
        BufferToWrite[1] = PointerByte[1];
        BufferToWrite[2] = PointerByte[2];
        BufferToWrite[3] = PointerByte[3];
        BytesToWrite = 4;
        Term_Update(BufferToWrite, BytesToWrite);
      }

      return 0; /* Do not send data back */
  }
  else if (!strncmp("setName ", (char *)att_data, 8))
  {
    int32_t NameLength = data_length - 1;
    int32_t i;

    if (NameLength > 8)
    {
      if ((NameLength - 8) > 7)
      {
        NameLength = 7;
        BytesToWrite = sprintf((char *)BufferToWrite, "NodeName too long\n");
        Term_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        NameLength = NameLength - 8;
      }

      for (i = 1; i < NameLength + 1; i++)
      {
        NodeName[i] = att_data[i + 7];
      }
      /* Fill the Remaining chars with ' ' */
      for (; i < 8; i++)
      {
        NodeName[i] = ' ';
      }

      MDM_SaveGMD(GMD_NODE_NAME, (void *)&NodeName);
      NecessityToSaveMetaDataManager = 1;

      BytesToWrite = sprintf((char *)BufferToWrite, "New NodeName= %s\n", NodeName);
      Term_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BytesToWrite = sprintf((char *)BufferToWrite, "Node Name missing\n");
      Term_Update(BufferToWrite, BytesToWrite);
    }

    return 0;
  }
  else if (!strncmp("setDate", (char *)(att_data), 7))
  {

    int32_t NameLength = data_length - 1;

    if (NameLength == 19)
    {
      RTC_DateTypeDef StartDate;

      StartDate.WeekDay = att_data[9] - 48;
      StartDate.Date = ((att_data[11] - 48) * 16) + (att_data[12] - 48);
      StartDate.Month = ((att_data[14] - 48) * 16) + (att_data[15] - 48);
      StartDate.Year = ((att_data[17] - 48) * 16) + (att_data[18] - 48);

      if (((StartDate.WeekDay > 0x00) && (StartDate.WeekDay < 0x08)) &&
          ((StartDate.Date > 0x00) && (StartDate.Date < 0x32)) &&
          ((StartDate.Month > 0x00) && (StartDate.Month < 0x13)) &&
          (StartDate.Year < 0x99))
      {
        /* Configure RTC Data */
        RTC_DateConfig(StartDate.WeekDay, StartDate.Date, StartDate.Month, StartDate.Year);
        BytesToWrite = sprintf((char *)BufferToWrite, "Date format Correct\n");
        Term_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BytesToWrite = sprintf((char *)BufferToWrite, "Date format not correct\n");
        Term_Update(BufferToWrite, BytesToWrite);
        BytesToWrite = sprintf((char *)BufferToWrite, "setDate wd/dd/mm/yy\n");
        Term_Update(BufferToWrite, BytesToWrite);
      }
    }
    return 0;
  }
  else if (!strncmp("setTime", (char *)(att_data), 7))
  {
    int32_t NameLength = data_length - 1;

    if (NameLength == 16)
    {
      RTC_TimeTypeDef StartTime;

      StartTime.Hours = ((att_data[8] - 48) * 16) + (att_data[9] - 48);
      StartTime.Minutes = ((att_data[11] - 48) * 16) + (att_data[12] - 48);
      StartTime.Seconds = ((att_data[14] - 48) * 16) + (att_data[15] - 48);

      if ((StartTime.Hours < 0x24) &&
          (StartTime.Minutes < 0x60) &&
          (StartTime.Seconds < 0x60))
      {
        /* Configure RTC Time */
        RTC_TimeConfig(StartTime.Hours, StartTime.Minutes, StartTime.Seconds);
        BytesToWrite = sprintf((char *)BufferToWrite, "Time format Correct\n");
        Term_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BytesToWrite = sprintf((char *)BufferToWrite, "Time format not correct\n");
        Term_Update(BufferToWrite, BytesToWrite);
        BytesToWrite = sprintf((char *)BufferToWrite, "setTime hh:mm:ss\n");
        Term_Update(BufferToWrite, BytesToWrite);
      }
      return 0;
    }
  }
  else if(!strncmp("versionFw",(char *)(att_data),9))
  {
      BytesToWrite =sprintf((char *)BufferToWrite,"%s_%s_%c.%c.%c\r\n",
#if defined(STM32L476xx)
                            "L476",
#elif defined(STM32L475xx)
                            "L475",
#elif defined(STM32_SENSORTILEBOX)
                            "L4R9",
#else
#error "Undefined STM32 processor type"
#endif
                            SENSING1_PACKAGENAME,
                            SENSING1_VERSION_MAJOR,
                            SENSING1_VERSION_MINOR,
                            SENSING1_VERSION_PATCH);
      Term_Update(BufferToWrite,BytesToWrite);
      return 0;
  }

  /* Else, pass command and arguments to CLI Processing --------------------- */

  /* Remove Line Feed '\n' */
  if (att_data[data_length - 1] == '\n') {
    att_data[data_length - 1] = '\0';
  }

  /* Add new line to output */
  Term_Update((uint8_t *)"\r\n", 2);

  do {
    /* Send the command string to the command interpreter.  Any
    output generated by the command interpreter will be placed in the
    pcOutputString buffer. */
    /* TODO: Check command string is either null terminated or space */
    /* TODO: Create #define for output buffer size */
    xMoreDataToFollow = FreeRTOS_CLIProcessCommand(
      (char *)att_data,    /* The command string.*/
      (char *)BufferToWrite,   /* The output buffer. */
      256 /* The size of the output buffer. */
    );

    /* Write the output generated by the command interpreter to the
    console. */
    Term_Update(BufferToWrite, strlen((char *)BufferToWrite));
  } while (xMoreDataToFollow != 0);

  /* Do not echo back command */
  return 0;
}

/**
 * @brief  This function makes the parsing of the Configuration Commands
 * @param uint8_t *att_data attribute data
 * @param uint8_t data_length length of the data
 * @retval uint32_t SendItBack true/false
 */
static uint32_t ConfigCommandParsing(uint8_t * att_data, uint8_t data_length)
{
  uint32_t SendItBack = 1;

#ifndef USE_STM32L475E_IOT01
  FeatureMask = (att_data[3]) | (att_data[2]<<8) | (att_data[1]<<16) | (att_data[0]<<24);
  uint8_t Command = att_data[4];
  uint8_t Data    = att_data[5];

  switch (FeatureMask) {
  case FEATURE_MASK_ACC_EVENTS:
    /* Acc events */
#ifdef SENSING1_DEBUG_CONNECTION
    if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
      BytesToWrite =sprintf((char *)BufferToWrite,"Conf Sig F=%lx C=%c D=%x\n",FeatureMask,Command,Data);
      Term_Update(BufferToWrite,BytesToWrite);
    } else {
      SENSING1_PRINTF("Conf Sig F=%lx C=%c D=%x\r\n",FeatureMask,Command,Data);
    }
#endif /* SENSING1_DEBUG_CONNECTION */
    if (Data == 1 ) {
      MOTION_SENSOR_Enable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
    }
    switch(Command) {
#ifndef STM32_SENSORTILEBOX
      case 'm':
        /* Multiple Events */
        switch(Data) {
          case 1:
            EnableHWMultipleEvents();
            ResetHWPedometer();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)){
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWMultipleEvents();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)){
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
         }
        break;
#endif /* STM32_SENSORTILEBOX */
      case 'f':
        /* FreeFall */
        switch(Data) {
          case 1:
            EnableHWFreeFall();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)){
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWFreeFall();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
         }
      break;
      case 'd':
        /* Double Tap */
        switch(Data) {
          case 1:
            EnableHWDoubleTap();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWDoubleTap();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
        }
      break;
      case 's':
        /* Single Tap */
        switch(Data) {
          case 1:
            EnableHWSingleTap();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWSingleTap();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
        }
      break;
#ifndef STM32_SENSORTILEBOX
      case 'p':
        /* Pedometer */
        switch(Data) {
          case 1:
            EnableHWPedometer();
            ResetHWPedometer();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWPedometer();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
        }
       break;
#endif /* STM32_SENSORTILEBOX */
      case 'w':
        /* Wake UP */
        switch(Data) {
          case 1:
            EnableHWWakeUp();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWWakeUp();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
        }
       break;
       case 't':
         /* Tilt */
        switch(Data) {
          case 1:
            EnableHWTilt();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
          case 0:
            DisableHWTilt();
            if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
              Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
            }
            break;
        }
      break;
      case 'o' :
        /* Tilt */
        switch(Data) {
        case 1:
          EnableHWOrientation6D();
          if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
          }
          break;
        case 0:
          DisableHWOrientation6D();
          if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_CONF_EVENT)) {
            Config_Notify(FEATURE_MASK_ACC_EVENTS,Command,Data);
          }
          break;
        }
      break;
    }
    if (Data == 0 ) {
      MOTION_SENSOR_Disable(TargetBoardFeatures.HandleAccSensor, MOTION_ACCELERO);
    }
    break;
  }
#endif /* USE_STM32L475E_IOT01 */
  return SendItBack;
}

#ifndef SENSING1_BlueNRG2
/**
 * @brief  This function is called whenever there is an ACI event to be processed.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void *pckt Pointer to the ACI packet
 * @retval None
 */
/*#define TRACE_HCI_CB_PRINTF   SENSING1_PRINTF */
#define TRACE_HCI_CB_PRINTF(...)
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  if (hci_pckt->type != HCI_EVENT_PKT) {
    TRACE_HCI_CB_PRINTF("HCI_Event_CB: INVALID\n\r");
    return;
  }

  switch (event_pckt->evt) {
    /* Disconnection Complete HCI Event (0x05) */
    case EVT_DISCONN_COMPLETE:
      TRACE_HCI_CB_PRINTF("HCI_Event_CB: Disconnection Complete\n\r");
      GAP_DisconnectionComplete_CB();
      break;

    /* LE Controller specific events (0x3E) */
    case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;
      HCI_LE_Event_CB(evt);
      break;
    }

    /* Vendor Specific Events (0xFF) */
    case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      HCI_Vendor_Event_CB(blue_evt);
      break;
    }
  }
}

static void HCI_LE_Event_CB(evt_le_meta_event *le_meta_evt)
{
  TRACE_HCI_CB_PRINTF("HCI_Event_CB: LE Meta Event");

  switch (le_meta_evt->subevent) {

    /* LE Connection Complete event */
    case EVT_LE_CONN_COMPLETE:
    {
      TRACE_HCI_CB_PRINTF(" - LE Connection Complete\n\r");
      evt_le_connection_complete *cc = (void *)le_meta_evt->data;
      GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
      break;
    }

    /* Connection Update Complete event */
    case EVT_LE_CONN_UPDATE_COMPLETE:
      TRACE_HCI_CB_PRINTF(" - LE Connection Update Complete\n\r");
      /* The Controller process to update the connection has completed */
      /* Nothing to do */
      break;

    default:
      TRACE_HCI_CB_PRINTF(" - other (subevent = %#x)\n\r", le_meta_evt->subevent);
      break;
  }
}

static void HCI_Vendor_Event_CB(evt_blue_aci *blue_evt)
{
  TRACE_HCI_CB_PRINTF("HCI_Event_CB: vendor");

  switch (blue_evt->ecode) {
    /*  Read request or read blob request is received */
    case EVT_BLUE_GATT_READ_PERMIT_REQ:
    {
      TRACE_HCI_CB_PRINTF(" - read permit req\n\r");
      evt_gatt_read_permit_req *pr = (void *)blue_evt->data;
      Read_Request_CB(pr->attr_handle);
      break;
    }

    /* Client modifies attribute on the server */
    case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
    {
      evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1 *)blue_evt->data;
      TRACE_HCI_CB_PRINTF(" - attr modified (attr_handle = %#04x)\n\r", evt->attr_handle);
      Attribute_Modified_CB(evt->attr_handle, evt->att_data, evt->data_length);
      break;
    }

    default:
      TRACE_HCI_CB_PRINTF(" - others (ecode = %#x)\n\r", blue_evt->ecode);
      break;
  }
}
#endif /* SENSING1_BlueNRG2 */


#ifdef STM32_SENSORTILEBOX
/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{ 
  connection_handle = Connection_Handle;

  SENSING1_PRINTF(">>>>>>CONNECTED %x:%x:%x:%x:%x:%x\r\n",Peer_Address[5],Peer_Address[4],Peer_Address[3],Peer_Address[2],Peer_Address[1],Peer_Address[0]);

  ConnectionBleStatus=0;
  DisableHWFeatures();
  LedBlinkStop();
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{  
  /* No Device Connected */
  connection_handle =0;
  
  SENSING1_PRINTF("<<<<<<DISCONNECTED\r\n");

  /* Make the device connectable again. */
  set_connectable = TRUE;
  ConnectionBleStatus=0;

  DisableHWFeatures();

  /* Reset for any problem during FOTA update */
  SizeOfUpdateBlueFW = 0;

#if SENSING1_USE_DATALOG
  /* Close the Log if the user exit in a dirty way */

  if(SD_LogMems_Enabled) {
    SD_Card_FeaturesMask &= ~(FEATURE_MASK_TEMP1 |
                              FEATURE_MASK_TEMP2|
                              FEATURE_MASK_PRESS |
                              FEATURE_MASK_HUM |
                              FEATURE_MASK_ACC |
                              FEATURE_MASK_GRYO |
                              FEATURE_MASK_MAG);
    SD_CardLoggingMemsStop();
  }

  if(SD_LogAudio_Enabled) {
    /* For waiting the close of the MEMS/Annotation file */
    osDelay(100);

    SD_Card_FeaturesMask &= ~FEATURE_MASK_BLUEVOICE;
    SD_CardLoggingAudioStop();
  }
#endif /* SENSING1_USE_DATALOG */

  /************************/
  /* Stops all the Timers */
  /************************/
  /* Stop Timer For MotionAR */
  switch(HarAlgo) {
    case HAR_GMP_IDX:
      stopProc(ACTIVITY_GMP);
    break;
    case HAR_IGN_IDX:
      stopProc(ACTIVITY_IGN);
    break;
    case HAR_IGN_WSDM_IDX:
      stopProc(ACTIVITY_IGN_WSDM);
    break;
    default:
    break;
  }

  /* Stop Timer For Acc/Gyro/Mag */
  stopProc(MOTION);

  /* Stop Timer For Environmental */
  stopProc(ENV);

  /* Stop Timer For Audio Level*/
  stopProc(AUDIO_LEV);

  /* Stop Timer Battery Level*/
  stopProc(BATTERY_INFO);

}/* end hci_disconnection_complete_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_read_permit_req_event.
 * Description    : This event is given when a read request is received
 *                  by the server from the client.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
  Read_Request_CB(Attribute_Handle);    
}

/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event is given when an attribute change his value.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_Request_CB(Connection_Handle, Attr_Handle, Offset, Attr_Data_Length, Attr_Data);
}
#endif /* STM32_SENSORTILEBOX */
/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
