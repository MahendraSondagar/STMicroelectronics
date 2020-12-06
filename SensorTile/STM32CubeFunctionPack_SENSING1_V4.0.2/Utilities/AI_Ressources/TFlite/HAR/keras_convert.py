#!/usr/bin/env python
# coding: utf-8 

#   This software component is licensed by ST under BSD 3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause

"""Convert a Keras model to Tflite format."""

from __future__ import print_function

import os
import tensorflow as tf

def replace_ext(filename, ext):
    """"Changes the extension of the filename."""
    return "{}{}".format(os.path.splitext(filename)[0], ext)

def keras_to_tflite(model):
    """Freezes the Tensorflow graph as saves it as a .tflite file."""
    try:
        from tensorflow import lite
    except ImportError:
        raise ImportError(
            "Tensorflow Lite not installed; cannot convert model.")

    converter = lite.TFLiteConverter.from_keras_model_file(model)
    with open(replace_ext(model, ".tflite"), 'wb') as fid:
        fid.write(converter.convert())

def main():
    """ script entry point """
    import argparse

    parser = argparse.ArgumentParser(description='convert keras model to Tflite')
    parser.add_argument('model', type=str, help='model file')
    args = parser.parse_args()
    keras_to_tflite(args.model)

if __name__ == '__main__':
    main()
