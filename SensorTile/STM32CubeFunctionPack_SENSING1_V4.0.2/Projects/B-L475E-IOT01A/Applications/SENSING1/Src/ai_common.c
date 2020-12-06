
#ifdef __cplusplus
 extern "C" {
#endif
/**
  ******************************************************************************
  * @file           : ai_common.c
  * @brief          : Ai common
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ai_common.h"
#include "ai_datatypes_defines.h"
#include "main.h"
 
static const ai_network_entry_t networks[AI_MNETWORK_NUMBER] = {
    {
        .name = (const char *)AI_HAR_GMP_MODEL_NAME,
        .config = AI_HAR_GMP_DATA_CONFIG,
        .ai_get_info = ai_har_gmp_get_info,
        .ai_create = ai_har_gmp_create,
        .ai_destroy = ai_har_gmp_destroy,
        .ai_get_error = ai_har_gmp_get_error,
        .ai_init = ai_har_gmp_init,
        .ai_run = ai_har_gmp_run,
        .ai_forward = ai_har_gmp_forward,
        .ai_data_weights_get_default = ai_har_gmp_data_weights_get,
        .params = { AI_HAR_GMP_DATA_WEIGHTS(0),
                AI_HAR_GMP_DATA_ACTIVATIONS(0)},
        .extActBufferStartAddr = AI_HAR_GMP_DATA_ACTIVATIONS_START_ADDR,
        .actBufferSize = AI_HAR_GMP_DATA_ACTIVATIONS_SIZE

    },
    {
        .name = (const char *)AI_HAR_IGN_MODEL_NAME,
        .config = AI_HAR_IGN_DATA_CONFIG,
        .ai_get_info = ai_har_ign_get_info,
        .ai_create = ai_har_ign_create,
        .ai_destroy = ai_har_ign_destroy,
        .ai_get_error = ai_har_ign_get_error,
        .ai_init = ai_har_ign_init,
        .ai_run = ai_har_ign_run,
        .ai_forward = ai_har_ign_forward,
        .ai_data_weights_get_default = ai_har_ign_data_weights_get,
        .params = { AI_HAR_IGN_DATA_WEIGHTS(0),
                AI_HAR_IGN_DATA_ACTIVATIONS(0)},
        .extActBufferStartAddr = AI_HAR_IGN_DATA_ACTIVATIONS_START_ADDR,
        .actBufferSize = AI_HAR_IGN_DATA_ACTIVATIONS_SIZE
    },
    {
        .name = (const char *)AI_HAR_IGN_WSDM_MODEL_NAME,
        .config = AI_HAR_IGN_WSDM_DATA_CONFIG,
        .ai_get_info = ai_har_ign_wsdm_get_info,
        .ai_create = ai_har_ign_wsdm_create,
        .ai_destroy = ai_har_ign_wsdm_destroy,
        .ai_get_error = ai_har_ign_wsdm_get_error,
        .ai_init = ai_har_ign_wsdm_init,
        .ai_run = ai_har_ign_wsdm_run,
        .ai_forward = ai_har_ign_wsdm_forward,
        .ai_data_weights_get_default = ai_har_ign_wsdm_data_weights_get,
        .params = { AI_HAR_IGN_WSDM_DATA_WEIGHTS(0),
                AI_HAR_IGN_WSDM_DATA_ACTIVATIONS(0)},
        .extActBufferStartAddr = AI_HAR_IGN_WSDM_DATA_ACTIVATIONS_START_ADDR,
        .actBufferSize = AI_HAR_IGN_WSDM_DATA_ACTIVATIONS_SIZE
    },
    {
        .name = (const char *)AI_ASC_MODEL_NAME,
        .config = AI_ASC_DATA_CONFIG,
        .ai_get_info = ai_asc_get_info,
        .ai_create = ai_asc_create,
        .ai_destroy = ai_asc_destroy,
        .ai_get_error = ai_asc_get_error,
        .ai_init = ai_asc_init,
        .ai_run = ai_asc_run,
        .ai_forward = ai_asc_forward,
        .ai_data_weights_get_default = ai_asc_data_weights_get,
        .params = { AI_ASC_DATA_WEIGHTS(0),
                AI_ASC_DATA_ACTIVATIONS(0)},
        .extActBufferStartAddr = AI_ASC_DATA_ACTIVATIONS_START_ADDR,
        .actBufferSize = AI_ASC_DATA_ACTIVATIONS_SIZE
    },
};

struct network_instance {
     const ai_network_entry_t *entry;
     ai_handle handle;
     ai_network_params params;
};

/* Number of instance is aligned on the number of network */
AI_STATIC struct network_instance gnetworks[AI_MNETWORK_NUMBER] = {0};

AI_DECLARE_STATIC
ai_bool ai_mnetwork_is_valid(const char* name,
        const ai_network_entry_t *entry)
{
    if (name && (strlen(entry->name) == strlen(name)) &&
            (strncmp(entry->name, name, strlen(entry->name)) == 0))
        return true;
    return false;
}

AI_DECLARE_STATIC
struct network_instance *ai_mnetwork_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst))
            return inst;
        else if ((!inst) && (gnetworks[i].entry == NULL))
            return &gnetworks[i];
    }
    return NULL;
}

AI_DECLARE_STATIC
void ai_mnetwork_release_handle(struct network_instance *inst)
{
    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        if ((inst) && (&gnetworks[i] == inst)) {
            gnetworks[i].entry = NULL;
            return;
        }
    }
}

AI_API_ENTRY
const char* ai_mnetwork_find(const char *name, ai_int idx)
{
    const ai_network_entry_t *entry;

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry))
            return entry->name;
        else {
            if (!idx--)
                return entry->name;
        }
    }
    return NULL;
}


AI_API_ENTRY
ai_error ai_mnetwork_create(const char *name, ai_handle* network,
        const ai_buffer* network_config)
{
    const ai_network_entry_t *entry;
    const ai_network_entry_t *found = NULL;
    ai_error err;
    struct network_instance *inst = ai_mnetwork_handle(NULL);

    if (!inst) {
        err.type = AI_ERROR_ALLOCATION_FAILED;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    for (int i=0; i<AI_MNETWORK_NUMBER; i++) {
        entry = &networks[i];
        if (ai_mnetwork_is_valid(name, entry)) {
            found = entry;
            break;
        }
    }

    if (!found) {
        err.type = AI_ERROR_INVALID_PARAM;
        err.code = AI_ERROR_CODE_NETWORK;
        return err;
    }

    if (network_config == NULL)
        err = found->ai_create(network, found->config);
    else
        err = found->ai_create(network, network_config);
    if ((err.code == AI_ERROR_CODE_NONE) && (err.type == AI_ERROR_NONE)) {
        inst->entry = found;
        inst->handle = *network;
        *network = (ai_handle*)inst;
    }

    return err;
}

AI_API_ENTRY
ai_handle ai_mnetwork_destroy(ai_handle network)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        ai_handle hdl = inn->entry->ai_destroy(inn->handle);
        if (hdl != inn->handle) {
            ai_mnetwork_release_handle(inn);
            network = AI_HANDLE_NULL;
        }
    }
    return network;
}

AI_API_ENTRY
ai_bool ai_mnetwork_get_info(ai_handle network, ai_network_report* report)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_info(inn->handle, report);
    else
        return false;
}

AI_API_ENTRY
ai_error ai_mnetwork_get_error(ai_handle network)
{
    struct network_instance *inn;
    ai_error err;
    err.type = AI_ERROR_INVALID_PARAM;
    err.code = AI_ERROR_CODE_NETWORK;

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_get_error(inn->handle);
    else
        return err;
}

AI_API_ENTRY
ai_bool ai_mnetwork_init(ai_handle network, const ai_network_params* params)
{
    struct network_instance *inn;
    ai_network_params par;

    /* TODO: adding check ai_buffer activations/weights shape coherence */

    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn) {
        par = inn->entry->params;
        if (params->activations.n_batches)
            par.activations = params->activations;
        else
            par.activations.data = params->activations.data;
        if (params->params.n_batches)
            par.params = params->params;
        else
            par.params.data = inn->entry->ai_data_weights_get_default();
        return inn->entry->ai_init(inn->handle, &par);
    }
    else
        return false;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_run(ai_handle network, const ai_buffer* input,
        ai_buffer* output)
{
    struct network_instance* inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_run(inn->handle, input, output);
    else
        return 0;
}

AI_API_ENTRY
ai_i32 ai_mnetwork_forward(ai_handle network, const ai_buffer* input)
{
    struct network_instance *inn;
    inn =  ai_mnetwork_handle((struct network_instance *)network);
    if (inn)
        return inn->entry->ai_forward(inn->handle, input);
    else
        return 0;
}

AI_API_ENTRY
 int ai_mnetwork_get_private_handle(ai_handle network,
         ai_handle *phandle,
         ai_network_params *pparams)
 {
     struct network_instance* inn;
     inn =  ai_mnetwork_handle((struct network_instance *)network);
     if (inn && phandle && pparams) {
         *phandle = inn->handle;
         *pparams = inn->params;
         return 0;
     }
     else
         return -1;
 }

AI_API_ENTRY
int ai_mnetwork_get_ext_data_activations(ai_handle network,
         ai_u32 *add,
         ai_u32 *size)
 {
     struct network_instance* inn;
     inn =  ai_mnetwork_handle((struct network_instance *)network);
     if (inn && add && size) {
         *add = inn->entry->extActBufferStartAddr;
         *size = inn->entry->actBufferSize;
         return 0;
     }
     else
         return -1;
 }

/* -----------------------------------------------------------------------------
 * AI-related functions
 * -----------------------------------------------------------------------------
 */

static struct ai_network_exec_ctx {
    ai_handle handle;
    ai_network_report report;
} net_ctx[AI_MNETWORK_NUMBER] = {0};


#define AI_BUFFER_NULL(ptr_)  \
  AI_BUFFER_OBJ_INIT( \
    AI_BUFFER_FORMAT_NONE|AI_BUFFER_FMT_FLAG_CONST, \
    0, 0, 0, 0, \
    AI_HANDLE_PTR(ptr_))


AI_ALIGNED(4)
static ai_u8 activations[AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE];

/* -----------------------------------------------------------------------------
 * AI-related functions
 * -----------------------------------------------------------------------------
 */



void aiLogErr(const ai_error err, const char *fct)
{
    if (fct)
        SENSING1_PRINTF("E: AI error (%s) - type=%d code=%d\r\n", fct,
                err.type, err.code);
    else
        SENSING1_PRINTF("E: AI error - type=%d code=%d\r\n", err.type, err.code);
}

ai_u32 aiBufferSize(const ai_buffer* buffer)
{
    return buffer->height * buffer->width * buffer->channels;
}

ai_u8* aiNetworkRetrieveDataWeightsAddress(uint32_t ActivationSize,uint8_t **ModelName)
{
  uint32_t NetworkNum=0;
  int32_t Found =-1;
  ai_u8 *Address=NULL;
  SENSING1_PRINTF("Searching the aiNetwork\r\n");
  while((Found==-1) & (NetworkNum<AI_MNETWORK_NUMBER)) {
    if(ActivationSize==networks[NetworkNum].params.activations.channels) {
      Found = NetworkNum;
    }
    NetworkNum++;
  }
  
  if(Found!=-1) {
    Address = networks[Found].ai_data_weights_get_default();
    *ModelName = (uint8_t*) networks[Found].name;
  }
  return Address;
}

__STATIC_INLINE void aiPrintLayoutBuffer(const char *msg, int idx,
        const ai_buffer* buffer)
{
    uint32_t type_id = AI_BUFFER_FMT_GET_TYPE(buffer->format);
    SENSING1_PRINTF("%s[%d] ",msg, idx);
    if (type_id == AI_BUFFER_FMT_TYPE_Q) {
        SENSING1_PRINTF(" %s%d,",
        		AI_BUFFER_FMT_GET_SIGN(buffer->format)?"s":"u",
                (int)AI_BUFFER_FMT_GET_BITS(buffer->format));
        if (AI_BUFFER_META_INFO_INTQ(buffer->meta_info)) {
            SENSING1_PRINTF(" scale=%f, zero=%d,",AI_BUFFER_META_INFO_INTQ_GET_SCALE(buffer->meta_info, 0), AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(buffer->meta_info, 0));
    	} else {
    		SENSING1_PRINTF("Q%d.%d,",
    				(int)AI_BUFFER_FMT_GET_BITS(buffer->format)
					- ((int)AI_BUFFER_FMT_GET_FBITS(buffer->format) +
					(int)AI_BUFFER_FMT_GET_SIGN(buffer->format)),
					AI_BUFFER_FMT_GET_FBITS(buffer->format));
    	}
    }
    else if (type_id == AI_BUFFER_FMT_TYPE_FLOAT)
        SENSING1_PRINTF(" float%d,",
                (int)AI_BUFFER_FMT_GET_BITS(buffer->format));
    else
        SENSING1_PRINTF("NONE");
    SENSING1_PRINTF(" %ld bytes, shape=(%d,%d,%ld)\r\n",
    		AI_BUFFER_BYTE_SIZE(AI_BUFFER_SIZE(buffer), buffer->format),
			buffer->height, buffer->width, buffer->channels);
}
__STATIC_INLINE void aiPrintNetworkInfo(const ai_network_report* report)
{
    int i;
    SENSING1_PRINTF("Network configuration...\r\n");
    SENSING1_PRINTF(" Model name         : %s\r\n", report->model_name);
    SENSING1_PRINTF(" Model signature    : %s\r\n", report->model_signature);
    SENSING1_PRINTF(" Model datetime     : %s\r\n", report->model_datetime);
    SENSING1_PRINTF(" Compile datetime   : %s\r\n", report->compile_datetime);
    SENSING1_PRINTF(" Runtime revision   : %d.%d.%d\r\n",
            report->runtime_version.major,
            report->runtime_version.minor,
            report->runtime_version.micro);
    SENSING1_PRINTF(" Tool revision      : %s (%d.%d.%d)\r\n", report->tool_revision,
            report->tool_version.major,
            report->tool_version.minor,
            report->tool_version.micro);
    SENSING1_PRINTF("Network info...\r\n");
    SENSING1_PRINTF("  nodes             : %ld\r\n", report->n_nodes);
    SENSING1_PRINTF("  complexity        : %ld MACC\r\n", report->n_macc);
    SENSING1_PRINTF("  activation        : %ld bytes\r\n",
            AI_BUFFER_SIZE(&report->activations));
    SENSING1_PRINTF("  params            : %ld bytes\r\n",
            AI_BUFFER_SIZE(&report->params));
    SENSING1_PRINTF("  inputs/outputs    : %u/%u\r\n", report->n_inputs,
            report->n_outputs);
    for (i=0; i<report->n_inputs; i++)
        aiPrintLayoutBuffer("   I", i, &report->inputs[i]);
    for (i=0; i<report->n_outputs; i++)
        aiPrintLayoutBuffer("   O", i, &report->outputs[i]);
}

/* -----------------------------------------------------------------------------
 * AI-related functions 2
 * -----------------------------------------------------------------------------
 */
int aiConvertInputFloat_2_Int8(const char *nn_name, const int idx, 
                               ai_float *In_f32, ai_i8 *Out_int8)
{
  if( AI_HANDLE_NULL == net_ctx[idx].handle)
  {
      SENSING1_PRINTF("E: network handle is NULL\r\n");
      return -1;
  }
  ai_buffer * bufferPtr   = &(net_ctx[idx].report.inputs[0]);
  ai_buffer_format format = bufferPtr->format;
  int size  = AI_BUFFER_SIZE(bufferPtr);
  ai_float scale ;
  int zero_point ;

  if (AI_BUFFER_FMT_TYPE_Q != AI_BUFFER_FMT_GET_TYPE(format) &&\
    ! AI_BUFFER_FMT_GET_SIGN(format) &&\
    8 != AI_BUFFER_FMT_GET_BITS(format))
  {
      SENSING1_PRINTF("E: expected signed integer 8 bits\r\n");
      return -1;
  }
  if (AI_BUFFER_META_INFO_INTQ(bufferPtr->meta_info)) {
      scale = AI_BUFFER_META_INFO_INTQ_GET_SCALE(bufferPtr->meta_info, 0);
      if (scale != 0.0F)
      {
         scale= 1.0F/scale ;
      }
      else 
      {
        SENSING1_PRINTF("E: division by zero\r\n");
        return -1;
      }   
      zero_point = AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(bufferPtr->meta_info, 0);
  } else {
      SENSING1_PRINTF("E: no meta info\r\n");
      return -1;
  }
  
  for (int i = 0; i < size ; i++)
  {
    Out_int8[i] = __SSAT((int32_t) roundf((float)zero_point + In_f32[i]*scale), 8);
  }
  return 0; 
}
int aiConvertOutputInt8_2_Float(const char *nn_name, const int idx,
                                ai_i8 *In_int8, ai_float *Out_f32)
{
  if( AI_HANDLE_NULL == net_ctx[idx].handle)
  {
      SENSING1_PRINTF("E: network handle is NULL\r\n");
      return -1;
  }
  ai_buffer * bufferPtr   = &(net_ctx[idx].report.outputs[0]);
  ai_buffer_format format = bufferPtr->format;
  int size  = AI_BUFFER_SIZE(bufferPtr);
  ai_float scale ;
  int zero_point ;

  if (AI_BUFFER_FMT_TYPE_Q != AI_BUFFER_FMT_GET_TYPE(format) &&\
    ! AI_BUFFER_FMT_GET_SIGN(format) &&\
    8 != AI_BUFFER_FMT_GET_BITS(format))
  {
      SENSING1_PRINTF("E: expected signed integer 8 bits\r\n");
      return -1;
  }
  if (AI_BUFFER_META_INFO_INTQ(bufferPtr->meta_info)) {
      scale = AI_BUFFER_META_INFO_INTQ_GET_SCALE(bufferPtr->meta_info, 0);
      zero_point = AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(bufferPtr->meta_info, 0);
  } else {
      SENSING1_PRINTF("E: no meta info\r\n");
      return -1;
  }
  
  for (uint32_t i = 0; i < size ; i++)
  {
    Out_f32[i] = scale * ((ai_float)(In_int8[i]) - zero_point);
  }
  return 0; 
}

int aiRun(const char *nn_name, const int idx, void *in_data, void *out_data)
{
  ai_buffer ai_input[1];
  ai_buffer ai_output[1];
  ai_i32 batch;
  ai_error err;

  if( AI_HANDLE_NULL == net_ctx[idx].handle)
  {
      SENSING1_PRINTF("E: network handle is NULL\r\n");
      return -1;
  }

  ai_input[0] = net_ctx[idx].report.inputs[0];
  ai_input[0].n_batches  = 1;
  ai_input[0].data = AI_HANDLE_PTR(in_data);

  ai_output[0] = net_ctx[idx].report.outputs[0];
  ai_output[0].n_batches = 1;
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  batch = ai_mnetwork_run(net_ctx[idx].handle, &ai_input[0], &ai_output[0]);
  if (batch != 1) {
      err = ai_mnetwork_get_error(net_ctx[idx].handle);
      aiLogErr(err,"ai_mnetwork_run");
      return  -1;
  }
  return 0;
}

int aiInit(const char *nn_name, const int idx)
{
  ai_error err;
  ai_u32 ext_addr =0 , sz=0;
 
  /* Creating the network */
  SENSING1_PRINTF("Creating the network \"%s\"..\r\n", nn_name);
  err = ai_mnetwork_create(nn_name, &net_ctx[idx].handle, NULL);
  if (err.type) {
      aiLogErr(err, "ai_mnetwork_create");
      return -1;
  }

  /* Query the created network to get relevant info from it */
  if (ai_mnetwork_get_info(net_ctx[idx].handle, &net_ctx[idx].report)) {
      aiPrintNetworkInfo(&net_ctx[idx].report);
  } else {
      err = ai_mnetwork_get_error(net_ctx[idx].handle);
      aiLogErr(err, "ai_mnetwork_get_info");
      ai_mnetwork_destroy(net_ctx[idx].handle);
      net_ctx[idx].handle = AI_HANDLE_NULL;
      return -1;
  }

  /* Initialize the instance */
  SENSING1_PRINTF("Initializing the network %s\r\n",nn_name);
  /* build params structure to provide the reference of the
   * activation and weight buffers */
#if !defined(AI_MNETWORK_DATA_ACTIVATIONS_INT_SIZE)
    const ai_network_params params = {
            AI_BUFFER_NULL(NULL),
            AI_BUFFER_NULL(activations) };
#else
    ai_network_params params = {
                AI_BUFFER_NULL(NULL),
                AI_BUFFER_NULL(NULL) };

    if (ai_mnetwork_get_ext_data_activations(net_ctx[idx].handle, &ext_addr, &sz) == 0) {
    	if (ext_addr == 0xFFFFFFFF) {
    		params.activations.data = (ai_handle)activations;
    		ext_addr = (ai_u32)activations;
    		sz = (ai_u32)AI_BUFFER_SIZE(&net_ctx[idx].report.activations);
    	}
    	else {
    		params.activations.data = (ai_handle)ext_addr;
    	}
    }
#endif

    SENSING1_PRINTF(" Activation buffer  : 0x%lx (%d bytes) %s\r\n", ext_addr, (int)sz,
    		(((uint32_t)(&net_ctx) & (ai_u32)0xFF000000) ==
    				((ai_u32)ext_addr & (ai_u32)0xFF000000))?"internal":"external");
                    
  if (!ai_mnetwork_init(net_ctx[idx].handle, &params)) {
      err = ai_mnetwork_get_error(net_ctx[idx].handle);
      aiLogErr(err, "ai_mnetwork_init");
      ai_mnetwork_destroy(net_ctx[idx].handle);
      net_ctx[idx].handle = AI_HANDLE_NULL;
      return -1;
  }
  return 0;
}
int aiDeInit(const char *nn_name, const int idx)
{
  SENSING1_PRINTF("Releasing the network %s...\r\n",nn_name);

  if (net_ctx[idx].handle) {
      if (ai_mnetwork_destroy(net_ctx[idx].handle) != AI_HANDLE_NULL) {
          aiLogErr(ai_mnetwork_get_error(net_ctx[idx].handle), "ai_mnetwork_destroy");
      }
      net_ctx[idx].handle = NULL;
      return -1;
  }
  return 0;
}
const ai_network_report* aiGetReport(const int idx)
{
  return(&net_ctx[idx].report);
}

#ifdef __cplusplus
}
#endif
