/**
  ******************************************************************************
  * @file    asc.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Tue Oct 15 11:25:05 2019
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */



#include "asc.h"

#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "layers.h"

#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#define AI_TOOLS_VERSION_MAJOR 4
#define AI_TOOLS_VERSION_MINOR 1
#define AI_TOOLS_VERSION_MICRO 0


#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#define AI_TOOLS_API_VERSION_MAJOR 1
#define AI_TOOLS_API_VERSION_MINOR 3
#define AI_TOOLS_API_VERSION_MICRO 0

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_asc
 
#undef AI_ASC_MODEL_SIGNATURE
#define AI_ASC_MODEL_SIGNATURE     "637c147537def27e0f4c918395f2d760"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     "(rev-4.1.0)"
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Tue Oct 15 11:25:05 2019"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_ASC_N_BATCHES
#define AI_ASC_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network array declarations  **************************************/
AI_STATIC ai_array conv2d_2_scratch1_array;   /* Array #0 */
AI_STATIC ai_array conv2d_2_scratch0_array;   /* Array #1 */
AI_STATIC ai_array conv2d_0_scratch1_array;   /* Array #2 */
AI_STATIC ai_array conv2d_0_scratch0_array;   /* Array #3 */
AI_STATIC ai_array dense_5_bias_array;   /* Array #4 */
AI_STATIC ai_array dense_5_weights_array;   /* Array #5 */
AI_STATIC ai_array dense_4_bias_array;   /* Array #6 */
AI_STATIC ai_array dense_4_weights_array;   /* Array #7 */
AI_STATIC ai_array conv2d_2_bias_array;   /* Array #8 */
AI_STATIC ai_array conv2d_2_weights_array;   /* Array #9 */
AI_STATIC ai_array conv2d_0_bias_array;   /* Array #10 */
AI_STATIC ai_array conv2d_0_weights_array;   /* Array #11 */
AI_STATIC ai_array input_0_output_array;   /* Array #12 */
AI_STATIC ai_array conv2d_0_output_array;   /* Array #13 */
AI_STATIC ai_array conv2d_2_output_array;   /* Array #14 */
AI_STATIC ai_array dense_4_output_array;   /* Array #15 */
AI_STATIC ai_array dense_5_output_array;   /* Array #16 */
AI_STATIC ai_array dense_5_fmt_output_array;   /* Array #17 */
AI_STATIC ai_array nl_6_output_array;   /* Array #18 */
AI_STATIC ai_array nl_6_fmt_output_array;   /* Array #19 */


/**  Forward network tensor declarations  *************************************/
AI_STATIC ai_tensor conv2d_2_scratch1;   /* Tensor #0 */
AI_STATIC ai_tensor conv2d_2_scratch0;   /* Tensor #1 */
AI_STATIC ai_tensor conv2d_0_scratch1;   /* Tensor #2 */
AI_STATIC ai_tensor conv2d_0_scratch0;   /* Tensor #3 */
AI_STATIC ai_tensor dense_5_bias;   /* Tensor #4 */
AI_STATIC ai_tensor dense_5_weights;   /* Tensor #5 */
AI_STATIC ai_tensor dense_4_bias;   /* Tensor #6 */
AI_STATIC ai_tensor dense_4_weights;   /* Tensor #7 */
AI_STATIC ai_tensor conv2d_2_bias;   /* Tensor #8 */
AI_STATIC ai_tensor conv2d_2_weights;   /* Tensor #9 */
AI_STATIC ai_tensor conv2d_0_bias;   /* Tensor #10 */
AI_STATIC ai_tensor conv2d_0_weights;   /* Tensor #11 */
AI_STATIC ai_tensor input_0_output;   /* Tensor #12 */
AI_STATIC ai_tensor conv2d_0_output;   /* Tensor #13 */
AI_STATIC ai_tensor conv2d_2_output;   /* Tensor #14 */
AI_STATIC ai_tensor conv2d_2_output0;   /* Tensor #15 */
AI_STATIC ai_tensor dense_4_output;   /* Tensor #16 */
AI_STATIC ai_tensor dense_5_output;   /* Tensor #17 */
AI_STATIC ai_tensor dense_5_fmt_output;   /* Tensor #18 */
AI_STATIC ai_tensor nl_6_output;   /* Tensor #19 */
AI_STATIC ai_tensor nl_6_fmt_output;   /* Tensor #20 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain conv2d_0_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain conv2d_2_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain dense_4_chain;   /* Chain #2 */
AI_STATIC_CONST ai_tensor_chain dense_5_chain;   /* Chain #3 */
AI_STATIC_CONST ai_tensor_chain dense_5_fmt_chain;   /* Chain #4 */
AI_STATIC_CONST ai_tensor_chain nl_6_chain;   /* Chain #5 */
AI_STATIC_CONST ai_tensor_chain nl_6_fmt_chain;   /* Chain #6 */


/**  Subgraph network operator tensor chain declarations  *********************/


/**  Subgraph network operator declarations  *********************************/


/**  Forward network layer declarations  **************************************/
AI_STATIC ai_layer_conv2d_nl_pool conv2d_0_layer; /* Layer #0 */
AI_STATIC ai_layer_conv2d_nl_pool conv2d_2_layer; /* Layer #1 */
AI_STATIC ai_layer_dense dense_4_layer; /* Layer #2 */
AI_STATIC ai_layer_dense dense_5_layer; /* Layer #3 */
AI_STATIC ai_layer_nl dense_5_fmt_layer; /* Layer #4 */
AI_STATIC ai_layer_nl nl_6_layer; /* Layer #5 */
AI_STATIC ai_layer_nl nl_6_fmt_layer; /* Layer #6 */


/**  Array declarations section  **********************************************/
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_scratch1_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 416,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 800,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_0_scratch1_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 960,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_0_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 260,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_5_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 3,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_5_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 27,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_4_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 9,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_4_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 5184,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2304,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_0_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_0_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 144,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  input_0_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 960,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 3360,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 576,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_4_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 9,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_5_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 3,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  dense_5_fmt_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  nl_6_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3,
  AI_STATIC)
AI_ARRAY_OBJ_DECLARE(
  nl_6_fmt_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 3,
  AI_STATIC)


AI_STATIC ai_intq_info_list conv2d_2_scratch1_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03457961976528168f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #0 */
AI_STATIC ai_intq_info_list conv2d_0_scratch1_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0313027985394001f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #1 */
AI_STATIC ai_intq_info_list dense_5_bias_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.00033943276503123343f), AI_PACK_INTQ_ZP(0)));   /* Int quant #2 */
AI_STATIC ai_intq_info_list dense_5_weights_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.00805626530200243f), AI_PACK_INTQ_ZP(0)));   /* Int quant #3 */
AI_STATIC ai_intq_info_list dense_4_bias_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(7.282017031684518e-05f), AI_PACK_INTQ_ZP(0)));   /* Int quant #4 */
AI_STATIC ai_intq_info_list dense_4_weights_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002105869585648179f), AI_PACK_INTQ_ZP(0)));   /* Int quant #5 */
AI_STATIC ai_intq_info_list conv2d_2_bias_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(7.796467980369925e-05f, 5.62530112802051e-05f, 7.363253826042637e-05f, 0.00011040033132303506f, 8.050013275351375e-05f, 7.262620783876628e-05f, 7.130693848012015e-05f, 6.488173676189035e-05f, 6.589482654817402e-05f, 5.89131195738446e-05f, 5.5870204960228875e-05f, 6.991926784394309e-05f, 6.229743303265423e-05f, 0.00011711788101820275f, 9.356316149933264e-05f, 9.771758777787909e-05f), AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));   /* Int quant #6 */
AI_STATIC ai_intq_info_list conv2d_2_weights_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002490661572664976f, 0.001797060132957995f, 0.002352267038077116f, 0.0035268517676740885f, 0.0025716593954712152f, 0.0023201187141239643f, 0.0022779733408242464f, 0.0020727135706692934f, 0.0021050777286291122f, 0.0018820400582626462f, 0.001784830936230719f, 0.0022336426191031933f, 0.001990155316889286f, 0.0037414508406072855f, 0.002988971071317792f, 0.0031216884963214397f), AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));   /* Int quant #7 */
AI_STATIC ai_intq_info_list conv2d_0_bias_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0002283232461195439f, 0.0001650891499593854f, 9.80427794274874e-05f, 0.0001889029226731509f, 0.00020746268273796886f, 0.00012143362255301327f, 0.0001230961352121085f, 0.00018371242913417518f, 0.00023007491836324334f, 0.00013137458881828934f, 0.00012431360664777458f, 0.00018123173504136503f, 8.639987208880484e-05f, 0.00015482305025216192f, 0.00015951036766637117f, 0.0001936932821990922f), AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));   /* Int quant #8 */
AI_STATIC ai_intq_info_list conv2d_0_weights_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004477717913687229f, 0.003237614408135414f, 0.001922747353091836f, 0.0037046337965875864f, 0.004068614915013313f, 0.002381472382694483f, 0.002414076356217265f, 0.003602841403335333f, 0.004512070678174496f, 0.0025764277670532465f, 0.002437952673062682f, 0.003554191906005144f, 0.0016944146482273936f, 0.003036282956600189f, 0.003128207288682461f, 0.0037985790986567736f), AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));   /* Int quant #9 */
AI_STATIC ai_intq_info_list input_0_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.050990983843803406f), AI_PACK_INTQ_ZP(42)));   /* Int quant #10 */
AI_STATIC ai_intq_info_list conv2d_0_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0313027985394001f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #11 */
AI_STATIC ai_intq_info_list conv2d_2_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.03457961976528168f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #12 */
AI_STATIC ai_intq_info_list dense_4_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04213276877999306f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #13 */
AI_STATIC ai_intq_info_list dense_5_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.14965805411338806f), AI_PACK_INTQ_ZP(2)));   /* Int quant #14 */
AI_STATIC ai_intq_info_list nl_6_fmt_output_intq = AI_INTQ_INFO_LIST_OBJ_INIT(
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1, AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.00390625f), AI_PACK_INTQ_ZP(-128)));   /* Int quant #15 */


/**  Tensor declarations section  *********************************************/
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_scratch1, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 13, 2), AI_STRIDE_INIT(4, 1, 1, 16, 208),
  1, &conv2d_2_scratch1_array, &conv2d_2_scratch1_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_scratch0, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 800, 1, 1), AI_STRIDE_INIT(4, 1, 1, 800, 800),
  1, &conv2d_2_scratch0_array, NULL)
AI_TENSOR_OBJ_DECLARE(
  conv2d_0_scratch1, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 30, 2), AI_STRIDE_INIT(4, 1, 1, 16, 480),
  1, &conv2d_0_scratch1_array, &conv2d_0_scratch1_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_0_scratch0, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 260, 1, 1), AI_STRIDE_INIT(4, 1, 1, 260, 260),
  1, &conv2d_0_scratch0_array, NULL)
AI_TENSOR_OBJ_DECLARE(
  dense_5_bias, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &dense_5_bias_array, &dense_5_bias_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_5_weights, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 9, 3, 1, 1), AI_STRIDE_INIT(4, 1, 9, 27, 27),
  1, &dense_5_weights_array, &dense_5_weights_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_4_bias, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 9, 1, 1), AI_STRIDE_INIT(4, 4, 4, 36, 36),
  1, &dense_4_bias_array, &dense_4_bias_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_4_weights, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 576, 9, 1, 1), AI_STRIDE_INIT(4, 1, 576, 5184, 5184),
  1, &dense_4_weights_array, &dense_4_weights_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_bias, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &conv2d_2_bias_array, &conv2d_2_bias_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_weights, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 16, 3, 3, 16), AI_STRIDE_INIT(4, 1, 16, 48, 144),
  1, &conv2d_2_weights_array, &conv2d_2_weights_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_0_bias, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &conv2d_0_bias_array, &conv2d_0_bias_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_0_weights, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 3, 16), AI_STRIDE_INIT(4, 1, 1, 3, 9),
  1, &conv2d_0_weights_array, &conv2d_0_weights_intq)
AI_TENSOR_OBJ_DECLARE(
  input_0_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 1, 32, 30), AI_STRIDE_INIT(4, 1, 1, 1, 32),
  1, &input_0_output_array, &input_0_output_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_0_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 15, 14), AI_STRIDE_INIT(4, 1, 1, 16, 240),
  1, &conv2d_0_output_array, &conv2d_0_output_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 16, 6, 6), AI_STRIDE_INIT(4, 1, 1, 16, 96),
  1, &conv2d_2_output_array, &conv2d_2_output_intq)
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_output0, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 576, 1, 1), AI_STRIDE_INIT(4, 1, 1, 576, 576),
  1, &conv2d_2_output_array, &conv2d_2_output_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_4_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 9, 1, 1), AI_STRIDE_INIT(4, 1, 1, 9, 9),
  1, &dense_4_output_array, &dense_4_output_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_5_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 1, 1, 3, 3),
  1, &dense_5_output_array, &dense_5_output_intq)
AI_TENSOR_OBJ_DECLARE(
  dense_5_fmt_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &dense_5_fmt_output_array, NULL)
AI_TENSOR_OBJ_DECLARE(
  nl_6_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &nl_6_output_array, NULL)
AI_TENSOR_OBJ_DECLARE(
  nl_6_fmt_output, AI_STATIC,
  0x0, 0x0, AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 1, 1, 3, 3),
  1, &nl_6_fmt_output_array, &nl_6_fmt_output_intq)


/**  Layer declarations section  **********************************************/



AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&input_0_output),
  AI_TENSOR_LIST_ENTRY(&conv2d_0_output),
  AI_TENSOR_LIST_ENTRY(&conv2d_0_weights, &conv2d_0_bias, NULL),
  AI_TENSOR_LIST_ENTRY(&conv2d_0_scratch0, &conv2d_0_scratch1)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_0_layer, 0,
  OPTIMIZED_CONV2D_TYPE,
  conv2d_nl_pool, forward_conv2d_nl_pool_integer,
  &AI_NET_OBJ_INSTANCE, &conv2d_2_layer, AI_STATIC,
  .tensors = &conv2d_0_chain, 
  .groups = 1, 
  .nl_func = NULL, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = pool_func_mp_array_integer, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_2_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&conv2d_0_output),
  AI_TENSOR_LIST_ENTRY(&conv2d_2_output),
  AI_TENSOR_LIST_ENTRY(&conv2d_2_weights, &conv2d_2_bias, NULL),
  AI_TENSOR_LIST_ENTRY(&conv2d_2_scratch0, &conv2d_2_scratch1)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_2_layer, 2,
  OPTIMIZED_CONV2D_TYPE,
  conv2d_nl_pool, forward_conv2d_nl_pool_integer,
  &AI_NET_OBJ_INSTANCE, &dense_4_layer, AI_STATIC,
  .tensors = &conv2d_2_chain, 
  .groups = 1, 
  .nl_func = NULL, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = pool_func_mp_array_integer, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_4_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&conv2d_2_output0),
  AI_TENSOR_LIST_ENTRY(&dense_4_output),
  AI_TENSOR_LIST_ENTRY(&dense_4_weights, &dense_4_bias),
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_4_layer, 4,
  DENSE_TYPE,
  dense, forward_dense_integer,
  &AI_NET_OBJ_INSTANCE, &dense_5_layer, AI_STATIC,
  .tensors = &dense_4_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_5_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&dense_4_output),
  AI_TENSOR_LIST_ENTRY(&dense_5_output),
  AI_TENSOR_LIST_ENTRY(&dense_5_weights, &dense_5_bias),
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_5_layer, 5,
  DENSE_TYPE,
  dense, forward_dense_integer,
  &AI_NET_OBJ_INSTANCE, &dense_5_fmt_layer, AI_STATIC,
  .tensors = &dense_5_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_5_fmt_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&dense_5_output),
  AI_TENSOR_LIST_ENTRY(&dense_5_fmt_output),
  AI_TENSOR_LIST_EMPTY,
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_5_fmt_layer, 5,
  NL_TYPE,
  nl, node_convert,
  &AI_NET_OBJ_INSTANCE, &nl_6_layer, AI_STATIC,
  .tensors = &dense_5_fmt_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_6_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&dense_5_fmt_output),
  AI_TENSOR_LIST_ENTRY(&nl_6_output),
  AI_TENSOR_LIST_EMPTY,
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_6_layer, 6,
  NL_TYPE,
  nl, forward_sm,
  &AI_NET_OBJ_INSTANCE, &nl_6_fmt_layer, AI_STATIC,
  .tensors = &nl_6_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_6_fmt_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_ENTRY(&nl_6_output),
  AI_TENSOR_LIST_ENTRY(&nl_6_fmt_output),
  AI_TENSOR_LIST_EMPTY,
  AI_TENSOR_LIST_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_6_fmt_layer, 6,
  NL_TYPE,
  nl, node_convert,
  &AI_NET_OBJ_INSTANCE, &nl_6_fmt_layer, AI_STATIC,
  .tensors = &nl_6_fmt_chain, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 7836, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 5152, 1,
                     NULL),
  AI_TENSOR_LIST_IO_ENTRY(AI_FLAG_NONE, AI_ASC_IN_NUM, &input_0_output),
  AI_TENSOR_LIST_IO_ENTRY(AI_FLAG_NONE, AI_ASC_OUT_NUM, &nl_6_fmt_output),
  &conv2d_0_layer, 0, NULL)



AI_DECLARE_STATIC
ai_bool asc_configure_activations(
  ai_network* net_ctx, const ai_buffer* activation_buffer)
{
  AI_ASSERT(net_ctx &&  activation_buffer && activation_buffer->data)

  ai_ptr activations = AI_PTR(AI_PTR_ALIGN(activation_buffer->data, 4));
  AI_ASSERT(activations)
  AI_UNUSED(net_ctx)

  {
    /* Updating activations (byte) offsets */
    conv2d_2_scratch1_array.data = AI_PTR(activations + 800);
    conv2d_2_scratch1_array.data_start = AI_PTR(activations + 800);
    conv2d_2_scratch0_array.data = AI_PTR(activations + 0);
    conv2d_2_scratch0_array.data_start = AI_PTR(activations + 0);
    conv2d_0_scratch1_array.data = AI_PTR(activations + 260);
    conv2d_0_scratch1_array.data_start = AI_PTR(activations + 260);
    conv2d_0_scratch0_array.data = AI_PTR(activations + 0);
    conv2d_0_scratch0_array.data_start = AI_PTR(activations + 0);
    input_0_output_array.data = AI_PTR(NULL);
    input_0_output_array.data_start = AI_PTR(NULL);
    conv2d_0_output_array.data = AI_PTR(activations + 1792);
    conv2d_0_output_array.data_start = AI_PTR(activations + 1792);
    conv2d_2_output_array.data = AI_PTR(activations + 1216);
    conv2d_2_output_array.data_start = AI_PTR(activations + 1216);
    dense_4_output_array.data = AI_PTR(activations + 0);
    dense_4_output_array.data_start = AI_PTR(activations + 0);
    dense_5_output_array.data = AI_PTR(activations + 12);
    dense_5_output_array.data_start = AI_PTR(activations + 12);
    dense_5_fmt_output_array.data = AI_PTR(activations + 0);
    dense_5_fmt_output_array.data_start = AI_PTR(activations + 0);
    nl_6_output_array.data = AI_PTR(activations + 0);
    nl_6_output_array.data_start = AI_PTR(activations + 0);
    nl_6_fmt_output_array.data = AI_PTR(NULL);
    nl_6_fmt_output_array.data_start = AI_PTR(NULL);
    
  }
  return true;
}



AI_DECLARE_STATIC
ai_bool asc_configure_weights(
  ai_network* net_ctx, const ai_buffer* weights_buffer)
{
  AI_ASSERT(net_ctx &&  weights_buffer && weights_buffer->data)

  ai_ptr weights = AI_PTR(weights_buffer->data);
  AI_ASSERT(weights)
  AI_UNUSED(net_ctx)

  {
    /* Updating weights (byte) offsets */
    
    dense_5_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_5_bias_array.data = AI_PTR(weights + 7824);
    dense_5_bias_array.data_start = AI_PTR(weights + 7824);
    dense_5_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_5_weights_array.data = AI_PTR(weights + 7796);
    dense_5_weights_array.data_start = AI_PTR(weights + 7796);
    dense_4_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_4_bias_array.data = AI_PTR(weights + 7760);
    dense_4_bias_array.data_start = AI_PTR(weights + 7760);
    dense_4_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_4_weights_array.data = AI_PTR(weights + 2576);
    dense_4_weights_array.data_start = AI_PTR(weights + 2576);
    conv2d_2_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_2_bias_array.data = AI_PTR(weights + 2512);
    conv2d_2_bias_array.data_start = AI_PTR(weights + 2512);
    conv2d_2_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_2_weights_array.data = AI_PTR(weights + 208);
    conv2d_2_weights_array.data_start = AI_PTR(weights + 208);
    conv2d_0_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_0_bias_array.data = AI_PTR(weights + 144);
    conv2d_0_bias_array.data_start = AI_PTR(weights + 144);
    conv2d_0_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_0_weights_array.data = AI_PTR(weights + 0);
    conv2d_0_weights_array.data_start = AI_PTR(weights + 0);
  }

  return true;
}


/**  PUBLIC APIs SECTION  *****************************************************/

AI_API_ENTRY
ai_bool ai_asc_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if ( report && net_ctx )
  {
    ai_network_report r = {
      .model_name        = AI_ASC_MODEL_NAME,
      .model_signature   = AI_ASC_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = {AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR,
                            AI_TOOLS_API_VERSION_MICRO, 0x0},

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 501428,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .activations       = AI_STRUCT_INIT,
      .params            = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if ( !ai_platform_api_get_network_report(network, &r) ) return false;

    *report = r;
    return true;
  }

  return false;
}

AI_API_ENTRY
ai_error ai_asc_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_asc_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_handle ai_asc_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_asc_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if ( !net_ctx ) return false;

  ai_bool ok = true;
  ok &= asc_configure_weights(net_ctx, &params->params);
  ok &= asc_configure_activations(net_ctx, &params->activations);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_asc_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_asc_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}

#undef AI_ASC_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

