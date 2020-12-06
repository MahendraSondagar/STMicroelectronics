#!/usr/bin/env python
# coding: utf-8 

#   This software component is licensed by ST under BSD 3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause
  

"""
Optimze Full int8 - with reference dataset
Fully quantized model tflite ASC - TF 1.14.0ASC 3CL Training script from Pre calculated features.
"""

import numpy as np
import tensorflow as tf


# load ASC training Set as representative quantization dataset (100 samples)
# reduced 'dummy' data set is provided , a full representative one should be provided instead

x_train_dataset = np.load('Asc_quant_representative_data_dummy.npz')
x_train = x_train_dataset['x_train']

ASC_SHAPE = (30, 32, 1)
N_CLASSES = 3

    
def representative_dataset_gen():
  for i in range(len(x_train)):
    # Get sample input data as a numpy array in a method of your choosing.
    yield [x_train[i].reshape((-1, ) + ASC_SHAPE)]


converter = tf.lite.TFLiteConverter.from_keras_model_file("Session_keras_mod_93_Model.h5" )
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_dataset_gen
converter.target_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

print("\nConverting the model...", flush=True)

tflite_model = converter.convert()

open('asc_keras_mod_93_to_tflite_int8_xtrain.tflite','wb').write(tflite_model)