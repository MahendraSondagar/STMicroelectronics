/**
  ******************************************************************************
  * @file    asc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Audio Scene Classification algorithm
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
#include "asc_processing.h"
#include "SENSING1.h"
#include "ai_platform.h"
#include "ai_common.h"

#include "asc_featurescaler.h"
#include "stm32l4xx.h"
#include "asc_postprocessing.h"
#include "feature_extraction.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NFFT             FILL_BUFFER_SIZE
#define NMELS            30

#define SPECTROGRAM_ROWS NMELS
#define SPECTROGRAM_COLS 32

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static float32_t aSpectrogram[SPECTROGRAM_ROWS * SPECTROGRAM_COLS];
static float32_t aColBuffer[SPECTROGRAM_ROWS];
static uint32_t SpectrColIndex;
float32_t aWorkingBuffer1[NFFT];

static ASC_OutputTypeDef ClassificationCode = ASC_UNDEFINED;
static arm_rfft_fast_instance_f32 S_Rfft;
static MelFilterTypeDef           S_MelFilter;
static SpectrogramTypeDef         S_Spectr;
static MelSpectrogramTypeDef      S_MelSpectr;

static void Preprocessing_Init(void);
static void PowerTodB(float32_t *pSpectrogram);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Create and Init ASC Convolutional Neural Network
  *
  * @retval ASC Status
  */
ASC_StatusTypeDef ASC_Init(void)
{

  if (AI_ASC_IN_1_SIZE != (SPECTROGRAM_ROWS * SPECTROGRAM_COLS))
    return ASC_ERROR ;

  ClassificationCode = ASC_UNDEFINED;

  /* Configure Audio preprocessing */
  Preprocessing_Init();

 /* enabling CRC clock for using AI libraries (for checking if STM32
  microprocessor is used)*/
  __HAL_RCC_CRC_CLK_ENABLE();
  if (aiInit(AI_ASC_MODEL_NAME,AI_ASC_MODEL_CTX))
    return ASC_ERROR ;

  return ASC_OK;

}

/**
  * @brief  DeInit ASC Convolutional Neural Network
  *
  * @retval ASC Status
  */
ASC_StatusTypeDef ASC_DeInit(void)
{
  if (aiDeInit(AI_ASC_MODEL_NAME,AI_ASC_MODEL_CTX))
    return ASC_ERROR ;

  /* Disable CRC Clock */
  __HAL_RCC_CRC_CLK_DISABLE();

  return ASC_OK;
}

/**
 * @brief  Run Acoustic Scene Recognition (ASC) algorithm.
 * @note   This function needs to be executed multiple times to extract audio features
 *
 * @retval Classification result code
 */
ASC_OutputTypeDef ASC_Run(float32_t *pBuffer)
{
  ai_float dense_2_out[AI_ASC_OUT_1_SIZE] = {0.0, 0.0, 0.0};

  /* Create a Mel-scaled spectrogram column */
   MelSpectrogramColumn(&S_MelSpectr, pBuffer, aColBuffer);
  /* Reshape and copy into output spectrogram column */
  for (uint32_t i = 0; i < NMELS; i++) {
    aSpectrogram[i * SPECTROGRAM_COLS + SpectrColIndex] = aColBuffer[i];
  }
  SpectrColIndex++;

  if (SpectrColIndex == SPECTROGRAM_COLS)
  {
    SpectrColIndex = 0;

    /* Convert to LogMel-scaled Spectrogram */
    PowerTodB(aSpectrogram);

    /* Run AI Network */
    ASC_NN_Run(aSpectrogram, dense_2_out);

    /* AI Network post processing */
    ClassificationCode = ASC_PostProc(dense_2_out);

    return ClassificationCode;
  }
  else
  {
    return ASC_UNDEFINED;
  }

}

/**
 * @brief  Get classification code computed by the ASC algorithm
 *
 * @retval Classification result
 */
ASC_OutputTypeDef ASC_GetClassificationCode(void)
{
  return ClassificationCode;
}

/**
 * @brief      ASC Convolutional Neural Net inference
 * @param[in]  pSpectrogram The CNN feature input
 * @param[out] pNetworkOut  The CNN output
 *
  * @retval ASC Status
 */
ASC_StatusTypeDef ASC_NN_Run(float32_t *pSpectrogram, float32_t *pNetworkOut)
{
  ai_i8 AscNnOutput[AI_ASC_OUT_1_SIZE];
  ai_i8 AscNnInput[AI_ASC_IN_1_SIZE];
  
  /* Z-Score Scaling on input feature */
  for (uint32_t i = 0; i < SPECTROGRAM_ROWS * SPECTROGRAM_COLS; i++)
  {
    pSpectrogram[i] = (pSpectrogram[i] - featureScalerMean[i]) / featureScalerStd[i];
  }

  aiConvertInputFloat_2_Int8(AI_ASC_MODEL_NAME, AI_ASC_MODEL_CTX,pSpectrogram, AscNnInput);
  aiRun(AI_ASC_MODEL_NAME, AI_ASC_MODEL_CTX, AscNnInput,AscNnOutput);
  aiConvertOutputInt8_2_Float(AI_ASC_MODEL_NAME, AI_ASC_MODEL_CTX,AscNnOutput, pNetworkOut);

  return ASC_OK;
}

/**
 * @brief Initialize LogMel preprocessing
 * @param none
 * @retval none
 */
static void Preprocessing_Init(void)
{
  /* Init RFFT */
  arm_rfft_fast_init_f32(&S_Rfft, 1024);

  /* Init Spectrogram */
  S_Spectr.pRfft    = &S_Rfft;
  S_Spectr.Type     = SPECTRUM_TYPE_POWER;
  S_Spectr.pWindow  = (float32_t *) hannWin_1024;
  S_Spectr.SampRate = 16000;
  S_Spectr.FrameLen = 1024;
  S_Spectr.FFTLen   = 1024;
  S_Spectr.pScratch = aWorkingBuffer1;

  /* Init Mel filter */
  S_MelFilter.pStartIndices = (uint32_t *) melFiltersStartIndices_1024_30;
  S_MelFilter.pStopIndices  = (uint32_t *) melFiltersStopIndices_1024_30;
  S_MelFilter.pCoefficients = (float32_t *) melFilterLut_1024_30;
  S_MelFilter.NumMels       = 30;

  /* Init MelSpectrogram */
  S_MelSpectr.SpectrogramConf = &S_Spectr;
  S_MelSpectr.MelFilter       = &S_MelFilter;
}

/**
 * @brief      LogMel Spectrum Calculation when all columns are populated
 * @param      pSpectrogram  Mel-scaled power spectrogram
 * @retval     none
 */
static void PowerTodB(float32_t *pSpectrogram)
{
  float32_t max_mel_energy = 0.0f;
  uint32_t i;

  /* Find MelEnergy Scaling factor */
  for (i = 0; i < NMELS * SPECTROGRAM_COLS; i++) {
    max_mel_energy = (max_mel_energy > pSpectrogram[i]) ? max_mel_energy : pSpectrogram[i];
  }

  /* Scale Mel Energies */
  for (i = 0; i < NMELS * SPECTROGRAM_COLS; i++) {
    pSpectrogram[i] /= max_mel_energy;
  }

  /* Convert power spectrogram to decibel */
  for (i = 0; i < NMELS * SPECTROGRAM_COLS; i++) {
    pSpectrogram[i] = 10.0f * log10f(pSpectrogram[i]);
  }

  /* Threshold output to -80.0 dB */
  for (i = 0; i < NMELS * SPECTROGRAM_COLS; i++) {
    pSpectrogram[i] = (pSpectrogram[i] < -80.0f) ? (-80.0f) : (pSpectrogram[i]);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
