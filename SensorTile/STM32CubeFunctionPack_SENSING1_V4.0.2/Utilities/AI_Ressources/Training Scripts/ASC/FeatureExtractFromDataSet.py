#!/usr/bin/env python
# coding: utf-8 

#   This software component is licensed by ST under BSD 3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause
  

"""LogMel Spectrogram Calculation from a collection of annotated audio files."""

import numpy as np
import scipy.fftpack as fft
import wave
import librosa
from sklearn import preprocessing
import joblib

audioChannels = 1 #mono= True
samplingRate = 16000 #samples/sec
fftSamples = 1024
melBands = 30
spectrogramCols = 32
spectrogramsPerFile = 28 # a spectrogram every 1024ms

#   Paths
dataSetPath = './Dataset/'
featureSetsPath = './Dataset/FeatureSet/'

### 3 classes : 0 indoor, 1 outdoor, 2 in-vehicle
ascLabels_3cl = {
            'bus' : 2,
            'cafeRestaurant' : 0,
            'car' : 2,
            'cityCenter' : 1,
            'home' : 0,
            'office' : 0,
            'park' : 1,
            'residentialArea' : 1,
            'shoppingCenter' : 0,
            'subway' : 2,
            'train' :  2,
            'tramway' :  2,
}

def wave_load(filename):
    '''load a .wav file from the dataset and returns float normalized samples'''
    
    audio_file = wave.open(filename, 'rb')
    sample_rate = audio_file.getframerate()
    sample_width = audio_file.getsampwidth()
    number_of_channels = audio_file.getnchannels()
    number_of_frames = audio_file.getnframes()

    data = audio_file.readframes(number_of_frames)
    audio_file.close()
    
    y_wav_int = np.frombuffer(data, 'int16') # deprecated
    y_wav_float_librosa = librosa.util.buf_to_float(y_wav_int, n_bytes=2, dtype=np.float32)
    
    return y_wav_float_librosa

def create_col(y):
    '''calculates a single column of Spectrogram from 1024 audio samples'''
    assert y.shape == (1024,)

    # Create time-series window
    fft_window = librosa.filters.get_window('hann', 1024, fftbins=True)
    # fft_window = fft_window.astype(np.float32)
    assert fft_window.shape == (1024,), fft_window.shape

    # Hann window
    y_windowed = fft_window * y
    assert y_windowed.shape == (1024,), y_windowed.shape

    # FFT
    fft_out = fft.fft(y_windowed, axis=0)[:513]
    assert fft_out.shape == (513,), fft_out.shape

    # Power spectrum
    S_pwr = np.abs(fft_out)**2
    assert S_pwr.shape == (513,)

    # Generation of Mel Filter Banks
    mel_basis = librosa.filters.mel(16000, n_fft=1024, n_mels=30, htk=False)
    # mel_basis.astype(np.float32)
    assert mel_basis.shape == (30, 513)

    # Apply Mel Filter Banks
    S_mel = np.dot(mel_basis, S_pwr)
    # S_mel.astype(np.float32)
    assert S_mel.shape == (30,)

    return S_mel

def spectrogram_normalization_dB(Spectrogram):
    # Scale according to reference power?
    Spectrogram = Spectrogram / Spectrogram.max()
    # Convert to dB
    S_log_mel = librosa.power_to_db(Spectrogram, top_db=80.0)
    return S_log_mel


#Loading Annotated Text filelists.
trainFileSet = np.loadtxt(dataSetPath + 'TrainSet.txt',dtype='str')
validFileSet = np.loadtxt(dataSetPath + 'ValidSet.txt',dtype='str')
testFileSet  = np.loadtxt(dataSetPath + 'TestSet.txt',dtype='str')

#%%
################################       Training Set       #######################################

# Allocation of Training Set vectors
X_train = np.empty([len(trainFileSet)*spectrogramsPerFile,30,32], dtype='float32', order='C')
y_train = np.empty([len(trainFileSet)*spectrogramsPerFile],dtype='int32')


##Building Training X and Y  !!!
print ('Building Fetures for Training Set....')
for i in range(trainFileSet.shape[0]):
    print ('Opening audio file %s:\t %d of %d \n' % (trainFileSet[i,0],i+1,trainFileSet.shape[0]))
    sig = wave_load(dataSetPath + trainFileSet[i,0])    
    # Sequence segmentation
    frames = librosa.util.frame(sig, frame_length=1024, hop_length=512) # 1024,936 samples frame
    logMelsSequence = np.empty([melBands,frames.shape[1]],dtype='float32', order = 'C')
    
    #extracting spectrograms
    for j in range(frames.shape[1]):
        logMelsSequence[:,j] = create_col(frames[:,j])
    
    for k in range(spectrogramsPerFile):
        # Spectrogram extraction from current sequence
        S_mel = logMelsSequence[:,k*spectrogramCols:(k+1)*spectrogramCols]
        # Spectrogram Normalization and dB
        S_log_mel = spectrogram_normalization_dB(S_mel)
        # Add to the global Training nd vector
        X_train[i*spectrogramsPerFile+k] = S_log_mel
        y_train[i*spectrogramsPerFile+k] = ascLabels_3cl[trainFileSet[i,1]]
    


#Saving Training Set vectors
print ('Saving Features for Training Set: ......\n\n')
joblib.dump(X_train, featureSetsPath + 'X_training_noScale.pkl', compress=5)
joblib.dump(y_train, featureSetsPath + 'Y_training.pkl', compress=5)

#######       AST ASC sTIle Training Set Z-score Scaler builidng        #######

X_train_r = X_train.reshape(X_train.shape[0],30*32)
scaler = preprocessing.StandardScaler().fit(X_train_r)
#Scaler Saving     
print ('Saving Z-Score Scaler:......\n')
joblib.dump(scaler, featureSetsPath + 'zscore_scaler.pkl')

########                       Validation Set                  #######

# Allocation of Validation Set vectors
X_val = np.empty([len(validFileSet)*spectrogramsPerFile,30,32], dtype='float32', order='C')
y_val = np.empty([len(validFileSet)*spectrogramsPerFile],dtype='int32')


##Building Validation Set X and Y  !!!
print ('Building Fetures for Validation Set....\n\n')
for i in range(validFileSet.shape[0]):
    print ('Opening audio file %s:\t %d of %d \n' % (validFileSet[i,0],i+1,validFileSet.shape[0]))
    sig = wave_load(dataSetPath + validFileSet[i,0])    
    # Sequence segmentation
    frames = librosa.util.frame(sig, frame_length=1024, hop_length=512) # (1024,936)
    logMelsSequence = np.empty([melBands,frames.shape[1]],dtype='float32', order = 'C')
    
    #extracting spectrograms
    for j in range(frames.shape[1]):
        logMelsSequence[:,j] = create_col(frames[:,j])
    
    for k in range(spectrogramsPerFile):
        # Spectrogram extraction from current sequence
        S_mel = logMelsSequence[:,k*spectrogramCols:(k+1)*spectrogramCols]
        # Spectrogram Normalization and dB
        S_log_mel = spectrogram_normalization_dB(S_mel)
        # Add to the global Validation Set vectors
        X_val[i*spectrogramsPerFile+k] = S_log_mel
        y_val[i*spectrogramsPerFile+k] = ascLabels_3cl[validFileSet[i,1]]
    

#Saving Validation Set vectors
print ('Saving Feature Vectors for Validation Set: ......\n\n')
joblib.dump(X_val, featureSetsPath + 'X_validation_noScale.pkl', compress=5)
joblib.dump(y_val, featureSetsPath + 'Y_validation.pkl', compress=5)
    

########                             Test Set                    #######

# Allocation of Test Set vectors
X_test = np.empty([len(testFileSet)*spectrogramsPerFile,30,32], dtype='float32', order='C')
y_test = np.empty([len(testFileSet)*spectrogramsPerFile],dtype='int32')


##Building Test Set X and Y  !!!
print ('Building Fetures for Test Set....\n\n')
for i in range(testFileSet.shape[0]):
    print ('Opening audio file %s:\t %d of %d \n' % (testFileSet[i,0],i+1,testFileSet.shape[0]))
    sig = wave_load(dataSetPath + testFileSet[i,0])    
    # Sequence segmentation
    frames = librosa.util.frame(sig, frame_length=1024, hop_length=512) # (1024,936)
    logMelsSequence = np.empty([melBands,frames.shape[1]],dtype='float32', order = 'C')
    
    #extracting spectrograms
    for j in range(frames.shape[1]):
        logMelsSequence[:,j] = create_col(frames[:,j])
    
    for k in range(spectrogramsPerFile):
        # Spectrogram extraction from current sequence
        S_mel = logMelsSequence[:,k*spectrogramCols:(k+1)*spectrogramCols]
        # Spectrogram Normalization and dB
        S_log_mel = spectrogram_normalization_dB(S_mel)
        # Add to the global Test Set vectors
        X_test[i*spectrogramsPerFile+k] = S_log_mel
        y_test[i*spectrogramsPerFile+k] = ascLabels_3cl[testFileSet[i,1]]

#Saving Test Set vectors
print ('Saving Feature Vectors for Test Set: ......\n\n')
joblib.dump(X_test, featureSetsPath + 'X_test_noScale.pkl', compress=5)
joblib.dump(y_test, featureSetsPath + 'Y_test.pkl', compress=5)
