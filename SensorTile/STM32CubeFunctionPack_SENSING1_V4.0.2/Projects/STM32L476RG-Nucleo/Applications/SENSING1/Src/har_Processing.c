/**
 ******************************************************************************
 * @file    har_Processing.c
 * @author  Central LAB
 * @version V4.0.0
 * @date    30-Oct-2019
 * @brief   This file includes activity recognition interface functions
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
#include "har_Processing.h"
#include "har_Preprocessing.h"
#include "har_Postprocessing.h"
#ifdef TEST_IGN_WSDM
  #include "har_ProcessingTest.h"
#endif

/* Imported Variable ---------------------------------------------------------*/

/* exported Variable ---------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define AI_NETWORK_IN_GMP_HEIGHT  (24)
#define WINDOW_GMP_STEP           (16)
#define N_OVERLAPPING_GMP_WIN     ((ai_int)(AI_NETWORK_IN_GMP_HEIGHT / WINDOW_GMP_STEP) + 1)

/* Private Variable ----------------------------------------------------------*/
static HAR_output_t ActivityCode[HAR_ALGO_IDX_NUMBER];
static ai_size n_sample = 0;
static ai_float window_buffer[N_OVERLAPPING_GMP_WIN * HAR_IN_MAX_SIZE] = {0};
static ai_float aiHarOut[HAR_OUT_MAX_SIZE] = {0};

static const char * aiHarAlgoNames[HAR_ALGO_IDX_NUMBER] = {
  AI_HAR_GMP_MODEL_NAME,
  AI_HAR_IGN_MODEL_NAME,
  AI_HAR_IGN_WSDM_MODEL_NAME
};

static const int aiHarAlgoCtx[HAR_ALGO_IDX_NUMBER] = {
  AI_HAR_GMP_MODEL_CTX,
  AI_HAR_IGN_MODEL_CTX,
  AI_HAR_IGN_WSDM_MODEL_CTX
};

HAR_output_t HAR_run(MOTION_SENSOR_AxesRaw_t ACC_Value_Raw, HAR_algoIdx_t algo)
{
  HAR_input_t iDataIN;
  HAR_input_t iDataInPreProc;
  float factor = TargetBoardFeatures.AccSensiMultInG;

  int height   = aiGetReport(algo)->inputs[0].height;
  int width    = aiGetReport(algo)->inputs[0].width;
  int size     = height * width ;

  if (HAR_IGN_WSDM_IDX == algo)
  {
    factor *= FROM_G_TO_MS_2 ;
  }

  iDataIN.AccX = (float)ACC_Value_Raw.x * factor;
  iDataIN.AccY = (float)ACC_Value_Raw.y * factor;
  iDataIN.AccZ = (float)ACC_Value_Raw.z * factor;

  if (HAR_IGN_WSDM_IDX == algo)
  {
#ifdef TEST_IGN_WSDM
    HAR_GetTestSamples(&iDataIN);
#endif
    iDataInPreProc = gravity_rotate(&iDataIN);
  }
  else
  {
    iDataInPreProc = gravity_suppress_rotate(&iDataIN);
  }

  if ( HAR_GMP_IDX == algo)
  {

    /* add samples to each active window */
      ai_size n_window = n_sample / WINDOW_GMP_STEP, pos = n_sample % WINDOW_GMP_STEP;
      for (ai_size i = 0; i < N_OVERLAPPING_GMP_WIN; ++i) {
        /* avoid partial buffers at start */
        if (n_window < i) continue;

        ai_int win_idx = (n_window - i) % N_OVERLAPPING_GMP_WIN;
        ai_int index = pos + i * WINDOW_GMP_STEP;
        ai_int win_offset = win_idx * size;

        if (index < height) {
          ai_size j = win_offset + index * width ;
          window_buffer[j++] = iDataInPreProc.AccX;
          window_buffer[j++] = iDataInPreProc.AccY;
          window_buffer[j]   = iDataInPreProc.AccZ;
        }
      /* if buffer is full, run the network */
      if (index == (height - 1)) {
        aiRun(aiHarAlgoNames[algo], aiHarAlgoCtx[algo],&window_buffer[win_offset],aiHarOut);
        ActivityCode[algo] = har_postProc(aiHarOut,algo);
      }
    }
    ++n_sample;
  }
  else
  {
    /* add samples to each active window */
    window_buffer[n_sample++] = iDataInPreProc.AccX;
    window_buffer[n_sample++] = iDataInPreProc.AccY;
    window_buffer[n_sample++] = iDataInPreProc.AccZ;
//    printf("In  : %f %f %f\r\n",iDataIN.AccX,iDataIN.AccY,iDataIN.AccZ);
//    printf("Out : %f %f %f\r\n",iDataInPreProc.AccX,iDataInPreProc.AccY,iDataInPreProc.AccZ);
    if  ( n_sample >=  size)
    {
      aiRun(aiHarAlgoNames[algo], aiHarAlgoCtx[algo],window_buffer,aiHarOut);
      ActivityCode[algo] = har_postProc(aiHarOut,algo);
      n_sample = 0;
    }
  }
  return ActivityCode[algo];
}

/**
* @brief  Initialises MotionAR algorithm
* @param  None
* @retval 0 if initilazed OK, a negative value otherwise
*/

int8_t HAR_Initialize(HAR_algoIdx_t algo)
{
  ActivityCode[algo]    = HAR_NOACTIVITY;

  /* enabling CRC clock for using AI libraries (for checking if STM32
  microprocessor is used)*/
  __HAL_RCC_CRC_CLK_ENABLE();
  if (aiInit(aiHarAlgoNames[algo], aiHarAlgoCtx[algo]))
    return -1 ;

  har_postProcInit(algo);

#if (defined(TEST_IGN_WSDM))
  if (HAR_IGN_WSDM_IDX == algo)
  {
    HAR_GetTestSamples_Init();
  }
#endif
  return 0;
}

int8_t HAR_DeInitialize(HAR_algoIdx_t algo)
{
  if (algo == HAR_ALGO_IDX_NONE)
  {
    return -1;
  }

  if (aiDeInit(aiHarAlgoNames[algo], aiHarAlgoCtx[algo]))
    return -1 ;

  __HAL_RCC_CRC_CLK_DISABLE();

#if (defined(TEST_IGN_WSDM))
  if (HAR_IGN_WSDM_IDX == algo)
  {
    HAR_GetTestSamples_DeInit();
  }
#endif
  return 0;
}

/**
 * @brief  get latest activity code computed by Recognition Algorithm
 * @param  None
 * @retval activity index
 */
HAR_output_t HAR_get_Activity_Code(HAR_algoIdx_t algo)
{
  return ActivityCode[algo];
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
