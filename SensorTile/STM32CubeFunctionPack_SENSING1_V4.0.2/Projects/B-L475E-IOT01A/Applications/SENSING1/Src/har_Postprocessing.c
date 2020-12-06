/**
  ******************************************************************************
  * @file    har_Postprocessing.c
  * @author  Microcontroller Division Team
  * @version V4.0.0
  * @date    30-Oct-2019  
  * @brief   Postprocessing functions
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


/* Include ------------------------------------------------------------------*/
#include "har_Postprocessing.h"

/*
 **********************
 * FMLP Temporal filter
 **********************
 */
/* Defines ------------------------------------------------------------------*/
#define FILT_ALPHA        0.2f

#define TF_WINDOW         10
#define TF_WALK_WINDOW    7

#define TF_SLOW_THR       7
#define TF_WALK_SHORT_THR 2
#define TF_WALK_LONG_THR  5

#define AR_ID_STATIONARY_GMP       (uint8_t)(0x00)
#define AR_ID_WALKING_GMP          (uint8_t)(0x01)
#define AR_ID_JOGGING_GMP          (uint8_t)(0x02)
#define AR_ID_BIKING_GMP           (uint8_t)(0x03)
#define AR_ID_DRIVING_GMP          (uint8_t)(0x04)
#define AR_ID_NONE                 (uint8_t)(0xFF)

#define AR_ID_BIKING_IGN           (uint8_t)(0x00)
#define AR_ID_DRIVING_IGN          (uint8_t)(0x01)
#define AR_ID_JOGGING_IGN          (uint8_t)(0x02)
#define AR_ID_STATIONARY_IGN       (uint8_t)(0x03)
#define AR_ID_WALKING_IGN          (uint8_t)(0x04)

#define AR_ID_JOGGING_IGN_WSDM     (uint8_t)(0x00)
#define AR_ID_STAIRS_IGN_WSDM      (uint8_t)(0x01)
#define AR_ID_STATIONARY_IGN_WSDM  (uint8_t)(0x02)
#define AR_ID_WALKING_IGN_WSDM     (uint8_t)(0x03)

static float last_scores  [HAR_ALGO_IDX_NUMBER][HAR_OUT_MAX_SIZE];
static const  HAR_output_t AR_ID_MAPPER [HAR_ALGO_IDX_NUMBER][HAR_OUT_MAX_SIZE]=
{
  {HAR_STATIONARY,HAR_WALKING,HAR_JOGGING   ,HAR_BIKING    ,HAR_DRIVING   },
  {HAR_BIKING    ,HAR_DRIVING,HAR_JOGGING   ,HAR_STATIONARY,HAR_WALKING   },
  {HAR_JOGGING   ,HAR_STAIRS ,HAR_STATIONARY,HAR_WALKING   ,HAR_NOACTIVITY}
};

static uint8_t raw_predictions_GMP[TF_WINDOW] = {0};
static uint8_t last_prediction_GMP = AR_ID_STATIONARY_GMP;
static uint8_t index_GMP = 0;
static uint8_t raw_predictions_IGN[TF_WINDOW] = {0};
static uint8_t last_prediction_IGN = AR_ID_STATIONARY_IGN;
static uint8_t index_IGN = 0;

static  uint8_t fmlp_temporal_filter_GMP(uint8_t prediction)
{
  raw_predictions_GMP[index_GMP] = prediction;
  index_GMP = (index_GMP + 1) % TF_WINDOW;

  uint8_t update = 1;
  int16_t count = 0;
  if (prediction == AR_ID_BIKING_GMP || prediction == AR_ID_DRIVING_GMP) {
    for (uint8_t i = 0; i < TF_WINDOW; ++i) {
      count += (raw_predictions_GMP[i] == prediction);
    }
    update = (count > TF_SLOW_THR);
  }
#if !TF_RESTRICTED
  if (prediction == AR_ID_WALKING_GMP) {
    /* checks the last TF_WALK_WINDOW samples */
    for (uint8_t i = index_GMP + TF_WINDOW - TF_WALK_WINDOW; i < index_GMP + TF_WINDOW; ++i) {
      count += (raw_predictions_GMP[i % TF_WINDOW] == prediction);
    }
    int16_t walk_threshold =
      (last_prediction_GMP == AR_ID_STATIONARY_GMP) ? TF_WALK_LONG_THR : TF_WALK_SHORT_THR;
    update = (count > walk_threshold);
  }
#endif /* TF_RESTRICTED */

  if (update) last_prediction_GMP = prediction;
  return last_prediction_GMP;
}
static  uint8_t fmlp_temporal_filter_IGN(uint8_t prediction)
{
  raw_predictions_IGN[index_IGN] = prediction;
  index_IGN = (index_IGN + 1) % TF_WINDOW;

  uint8_t update = 1;
  int16_t count = 0;
  if (prediction == AR_ID_BIKING_IGN || prediction == AR_ID_DRIVING_IGN) {
    for (uint8_t i = 0; i < TF_WINDOW; ++i) {
      count += (raw_predictions_IGN[i] == prediction);
    }
    update = (count > TF_SLOW_THR);
  }
#if !TF_RESTRICTED
  if (prediction == AR_ID_WALKING_IGN) {
    /* checks the last TF_WALK_WINDOW samples */
    for (uint8_t i = index_IGN + TF_WINDOW - TF_WALK_WINDOW; i < index_IGN + TF_WINDOW; ++i) {
      count += (raw_predictions_IGN[i % TF_WINDOW] == prediction);
    }
    int16_t walk_threshold = 
      (last_prediction_IGN == AR_ID_STATIONARY_IGN) ? TF_WALK_LONG_THR : TF_WALK_SHORT_THR;
    update = (count > walk_threshold);
  }
#endif /* TF_RESTRICTED */

  if (update) last_prediction_IGN = prediction;
  return last_prediction_IGN;
}

/* Input : Array of float
   Return index of the most high value of the array
*/

static uint8_t argmax(const float * array, int size)
{
  float max = -1e9f;
  uint8_t max_idx = 0;
  for (int i = 0; i < size; ++i) {
    if (array[i] > max) {
      max = array[i]; max_idx = i;
    }
  }
  return max_idx;
}
void har_postProcInit(HAR_algoIdx_t algo)
{
  int i ;
  int size     = aiGetReport(algo)->outputs[0].channels;

  for (i = 0; i < size; ++i) {
    last_scores[algo][i] = 0.0F;
  }

  if (HAR_IGN_IDX == algo ) 
  {
    for (i = 0; i < TF_WINDOW; ++i) {
      raw_predictions_IGN [i] = 0.0F;
    }
    last_prediction_IGN = AR_ID_STATIONARY_IGN;
    index_IGN = 0; 
  }
  else if (HAR_GMP_IDX == algo ) 
  {
    for (i = 0; i < TF_WINDOW; ++i) {
      raw_predictions_GMP [i] = 0.0F;
    }
    last_prediction_GMP = AR_ID_STATIONARY_GMP;
    index_GMP = 0; 
  }
}

/* Exported Functions ---------------------------------------------*/
HAR_output_t har_postProc(float * scores,HAR_algoIdx_t algo)
{
  uint8_t predict, predictFilt;
  int size     = aiGetReport(algo)->outputs[0].channels ;

//  SENSING1_PRINTF("\r\n");
  for (int i = 0; i < size; ++i) {
    last_scores[algo][i] = (1.0f - FILT_ALPHA) * last_scores[algo][i] + FILT_ALPHA * scores[i];
//    SENSING1_PRINTF("(%f)%f ", scores[i], last_scores[algo][i]);
  }

  predict = argmax(last_scores[algo],size);

  if (HAR_IGN_WSDM_IDX == algo ) 
  {
    predictFilt = predict;
  }
  else if (HAR_IGN_IDX == algo ) 
  {
    predictFilt = fmlp_temporal_filter_IGN(predict);
  }
  else
  {
    predictFilt = fmlp_temporal_filter_GMP(predict);
  }
  return AR_ID_MAPPER[algo][predictFilt];
}


/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/