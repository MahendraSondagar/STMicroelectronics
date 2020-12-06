/**
 ******************************************************************************
 * @file    asc_postprocessing.c
 * @author  Central LAB
 * @version V4.0.0
 * @date    30-Oct-2019
 * @brief   Postprocessing for Audio Scene Classification algorithm
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
#include "asc_postprocessing.h"
#include "sensor_service.h"

/* Private typedef -----------------------------------------------------------*/
#define PP_FLT_LENGTH 7

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static float32_t ascNNOutBuffer0[PP_FLT_LENGTH];
static float32_t ascNNOutBuffer1[PP_FLT_LENGTH];
static float32_t ascNNOutBuffer2[PP_FLT_LENGTH];

extern uint8_t BufferToWrite[];
extern int32_t BytesToWrite;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief      ASC postprocessing
 * @param[in]  pNNOut  The CNN result
 * @retval     Classification result
 */
ASC_OutputTypeDef ASC_PostProc(float32_t *pNNOut)
{
  float32_t max_out;
  uint32_t classification_result;

  /* ASC NN Output FIFO buffers left shift */
  memmove(ascNNOutBuffer0, ascNNOutBuffer0 + 1, sizeof(float32_t) * (PP_FLT_LENGTH - 1));
  memmove(ascNNOutBuffer1, ascNNOutBuffer1 + 1, sizeof(float32_t) * (PP_FLT_LENGTH - 1));
  memmove(ascNNOutBuffer2, ascNNOutBuffer2 + 1, sizeof(float32_t) * (PP_FLT_LENGTH - 1));
  ascNNOutBuffer0[PP_FLT_LENGTH - 1] = pNNOut[0];
  ascNNOutBuffer1[PP_FLT_LENGTH - 1] = pNNOut[1];
  ascNNOutBuffer2[PP_FLT_LENGTH - 1] = pNNOut[2];

  /* Reset of network out to be filled with the average of the ASC out Buffer */
  pNNOut[0] = 0.0f;
  pNNOut[1] = 0.0f;
  pNNOut[2] = 0.0f;

  /* Averaging ASC NN Outputs over a Buffer of Filter Length inferences */
  for (uint32_t i = 0; i < PP_FLT_LENGTH; i++)
  {
    pNNOut[0] += ascNNOutBuffer0[i];
    pNNOut[1] += ascNNOutBuffer1[i];
    pNNOut[2] += ascNNOutBuffer2[i];
  }
  pNNOut[0] /= (float32_t) PP_FLT_LENGTH;
  pNNOut[1] /= (float32_t) PP_FLT_LENGTH;
  pNNOut[2] /= (float32_t) PP_FLT_LENGTH;

  /* ArgMax to associate NN output with the most likely classification label */
  max_out = pNNOut[0];
  classification_result = 0;
  for (uint32_t i = 1; i < 3; i++)
  {
    if (pNNOut[i] > max_out)
    {
      max_out = pNNOut[i];
      classification_result = i;
    }
  }

  if(W2ST_CHECK_CONNECTION(W2ST_CONNECT_STD_TERM)) {
    BytesToWrite = sprintf((char *)BufferToWrite,"NNconfidence = %ld%%\n", (int32_t)(max_out * 100));
    Term_Update(BufferToWrite,BytesToWrite);
  } else {
    SENSING1_PRINTF("ASC=   %ld%% %ld%% %ld%%\r\n", (int32_t) (pNNOut[0] * 100), (int32_t)(pNNOut[1] * 100), (int32_t)(pNNOut[2] * 100));
  }

  return (ASC_OutputTypeDef) classification_result;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
