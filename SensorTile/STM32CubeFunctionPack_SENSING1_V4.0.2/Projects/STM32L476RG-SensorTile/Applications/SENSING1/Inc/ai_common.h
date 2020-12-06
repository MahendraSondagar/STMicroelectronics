/**
  ******************************************************************************
  * @file    ai_common.h 
  * @author  Central LAB
  * @version V4.0.0
  * @date    30-Oct-2019
  * @brief   Utilities used for AI algorithms
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


#ifndef __AI_COMMON_H_
#define __AI_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TargetFeatures.h"
#include "ai_platform.h"
#include "asc.h"
#include "asc_data.h"
#include "har_gmp.h"
#include "har_gmp_data.h"
#include "har_ign_wsdm.h"
#include "har_ign_wsdm_data.h"
#include "har_ign.h"
#include "har_ign_data.h"

/* Multiple network support --------------------------------------------------*/

typedef struct {
    const char *name;
    ai_network_params params;
    ai_buffer *config;
    ai_handle (*ai_data_weights_get_default)(void);
    ai_bool (*ai_get_info)(ai_handle network, ai_network_report* report);
    ai_error (*ai_create)(ai_handle* network, const ai_buffer* network_config);
    ai_error (*ai_get_error)(ai_handle network);
    ai_handle (*ai_destroy)(ai_handle network);
    ai_bool (*ai_init)(ai_handle network, const ai_network_params* params);
    ai_i32 (*ai_run)(ai_handle network, const ai_buffer* input, ai_buffer* output);
    ai_i32 (*ai_forward)(ai_handle network, const ai_buffer* input);
    ai_u32 extActBufferStartAddr;
    ai_u32 actBufferSize;
} ai_network_entry_t;

#define AI_ASC_DATA_ACTIVATIONS_START_ADDR 0xFFFFFFFF
#define AI_HAR_GMP_DATA_ACTIVATIONS_START_ADDR 0xFFFFFFFF
#define AI_HAR_IGN_WSDM_DATA_ACTIVATIONS_START_ADDR 0xFFFFFFFF
#define AI_HAR_IGN_DATA_ACTIVATIONS_START_ADDR 0xFFFFFFFF

#define AI_MNETWORK_NUMBER  (4)

#define AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE AI_HAR_GMP_DATA_ACTIVATIONS_SIZE

#if AI_HAR_IGN_DATA_ACTIVATIONS_SIZE > AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#undef AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#define AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE AI_HAR_IGN_DATA_ACTIVATIONS_SIZE
#endif

#if AI_HAR_IGN_WSDM_DATA_ACTIVATIONS_SIZE > AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#undef AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#define AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE AI_HAR_IGN_WSDM_DATA_ACTIVATIONS_SIZE
#endif

#if AI_ASC_DATA_ACTIVATIONS_SIZE > AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#undef AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE
#define AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE AI_ASC_DATA_ACTIVATIONS_SIZE
#endif

AI_API_DECLARE_BEGIN

AI_API_ENTRY
const char* ai_mnetwork_find(const char *name, ai_int idx);

AI_API_ENTRY
ai_bool ai_mnetwork_get_info(
  ai_handle network, ai_network_report* report);

/*!
 * @brief Get first network error code.
 * @ingroup network
 * @details Get an error code related to the 1st error generated during
 * network processing. The error code is structure containing an
 * error type indicating the type of error with an associated error code
 * Note: after this call the error code is internally reset to AI_ERROR_NONE
 * @param network an opaque handle to the network context
 * @return an error type/code pair indicating both the error type and code
 * see @ref ai_error for struct definition
 */
AI_API_ENTRY
ai_error ai_mnetwork_get_error(ai_handle network);

/*!
 * @brief Create a neural network.
 * @ingroup network
 * @details Instantiate a network and returns an object to handle it;
 * @param network an opaque handle to the network context
 * @param network_config a pointer to the network configuration info coded as a
 * buffer
 * @return an error code reporting the status of the API on exit
 */
AI_API_ENTRY
ai_error ai_mnetwork_create(const char *name,
  ai_handle* network, const ai_buffer* network_config);

/*!
 * @brief Destroy a neural network and frees the allocated memory.
 * @ingroup network
 * @details Destroys the network and frees its memory. The network handle is returned;
 * if the handle is not NULL, the unloading has not been successful.
 * @param network an opaque handle to the network context
 * @return an object handle : AI_HANDLE_NULL if network was destroyed
 * correctly. The same input network handle if destroy failed.
 */
AI_API_ENTRY
ai_handle ai_mnetwork_destroy(ai_handle network);

/*!
 * @brief Initialize the data structures of the network.
 * @ingroup network
 * @details This API initialized the network after a successfull
 * @ref ai_network_create. Both the activations memory buffer
 * and params (i.e. weights) need to be provided by caller application
 *
 * @param network an opaque handle to the network context
 * @param params the parameters of the network (required).
 * see @ref ai_network_params struct for details
 * @return true if the network was correctly initialized, false otherwise
 * in case of error the error type could be queried by
 * using @ref ai_network_get_error
 */
AI_API_ENTRY
ai_bool ai_mnetwork_init(
  ai_handle network, const ai_network_params* params);

/*!
 * @brief Run the network and return the output
 * @ingroup network
 *
 * @details Runs the network on the inputs and returns the corresponding output.
 * The size of the input and output buffers is stored in this
 * header generated by the code generation tool. See AI_NETWORK_*
 * defines into file @ref network.h for all network sizes defines
 *
 * @param network an opaque handle to the network context
 * @param[in] input buffer with the input data
 * @param[out] output buffer with the output data
 * @return the number of input batches processed (default 1) or <= 0 if it fails
 * in case of error the error type could be queried by
 * using @ref ai_network_get_error
 */
AI_API_ENTRY
ai_i32 ai_mnetwork_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output);

/*!
 * @brief Runs the network on the inputs.
 * @ingroup network
 *
 * @details Differently from @ref ai_network_run, no output is returned, e.g. for
 * temporal models with a fixed step size.
 *
 * @param network the network to be run
 * @param[in] input buffer with the input data
 * @return the number of input batches processed (usually 1) or <= 0 if it fails
 * in case of error the error type could be queried by
 * using @ref ai_network_get_error
 */
AI_API_ENTRY
ai_i32 ai_mnetwork_forward(
  ai_handle network, const ai_buffer* input);

AI_API_ENTRY
int ai_mnetwork_get_private_handle(ai_handle network,
        ai_handle *phandle,
        ai_network_params* pparams);

AI_API_DECLARE_END

#define AI_HAR_GMP_MODEL_CTX      (0)
#define AI_HAR_IGN_MODEL_CTX      (1)
#define AI_HAR_IGN_WSDM_MODEL_CTX (2)
#define AI_ASC_MODEL_CTX          (3)

extern int aiRun(const char *nn_name, const int idx, void *in_data, 
                 void *out_data);
extern int aiInit(const char *nn_name, const int idx);
extern int aiDeInit(const char *nn_name, const int idx);
extern const ai_network_report* aiGetReport(const int idx);
extern int aiConvertInputFloat_2_Int8(const char *nn_name, const int idx, 
                                      ai_float *In_f32, ai_i8 *Out_int8);
extern int aiConvertOutputInt8_2_Float(const char *nn_name, const int idx,
                                       ai_i8 *In_int8, ai_float *Out_f32);
extern ai_u8* aiNetworkRetrieveDataWeightsAddress(uint32_t ActivationSize,
                                                  uint8_t **ModelName);

#ifdef __cplusplus
}
#endif

#endif /* __AI_COMMON_H_ */
