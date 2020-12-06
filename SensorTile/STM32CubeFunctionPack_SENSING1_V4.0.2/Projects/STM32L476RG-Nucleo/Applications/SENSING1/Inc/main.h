/**
  ******************************************************************************
  * @file    main.h 
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#ifndef STM32_SENSORTILEBOX
  #include "console.h"
#endif /* STM32_SENSORTILEBOX */
#include "hci.h"

#ifndef SENSING1_BlueNRG2
  #include "hci_le.h"
  #include "sm.h"
#endif /* SENSING1_BlueNRG2 */

#include "hci_tl.h"
#include "hci_tl_interface.h"
#include "SENSING1.h"
#include "TargetFeatures.h"
#include "har_Processing.h"
#include "sensor_service.h"

/* Exported macro ------------------------------------------------------------*/
#define MCR_BLUEMS_F2I_1D(in, out_int, out_dec) {out_int = (int32_t)in; out_dec= (int32_t)((in-out_int)*10);};
#define MCR_BLUEMS_F2I_2D(in, out_int, out_dec) {out_int = (int32_t)in; out_dec= (int32_t)((in-out_int)*100);};

/* Exported functions ------------------------------------------------------- */
extern void Error_Handler(void);
extern void Set2GAccelerometerFullScale(void);
extern void Set4GAccelerometerFullScale(void);
extern void AudioProcess_FromPC(uint8_t *pData, uint32_t count);
extern void ReadEnvironmentalData(int32_t *PressToSend,uint16_t *HumToSend,int16_t *Temp1ToSend,int16_t *Temp2ToSend);

/* Blinking Led functions */
extern void LedBlinkStart(void);
extern void LedBlinkStop(void);

extern unsigned char ReCallCalibrationFromMemory(uint16_t dataSize, uint32_t *data);
extern unsigned char SaveCalibrationToMemory(uint16_t dataSize, uint32_t *data);
extern int SendMsgToHost(msgData_t *mailPtr);

extern void RTC_DateConfig(uint8_t WeekDay, uint8_t Date, uint8_t Month, uint8_t Year);
extern void RTC_TimeConfig(uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
extern HAL_StatusTypeDef RTC_GetCurrentDateTime(void);

extern void SystemClock_Config(void);

/* Exported defines and variables  ------------------------------------------------------- */

#define RTC_CLOCK_SOURCE_LSI
/*#define RTC_CLOCK_SOURCE_LSE*/

extern uint8_t BufferToWrite[256];
extern int32_t BytesToWrite;
extern HAR_algoIdx_t HarAlgo;

extern RTC_DateTypeDef CurrentDate;
extern RTC_TimeTypeDef CurrentTime;

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
