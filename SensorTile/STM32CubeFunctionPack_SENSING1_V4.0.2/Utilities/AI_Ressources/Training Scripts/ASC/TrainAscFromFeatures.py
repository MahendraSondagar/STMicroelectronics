#!/usr/bin/env python
# coding: utf-8 

#   This software component is licensed by ST under BSD 3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause
  

"""ASC 3CL Training script from Pre calculated features."""

from __future__ import print_function

from time import strftime
import joblib
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix
from keras import layers
from keras import models
from keras import optimizers
from keras.utils import to_categorical

# Test Session Parameters
featureSetsPath = './Dataset/FeatureSet/'
logPath = './Output/'
sessionNum = 'Dummy'
spectrogramRows = 30
spectrogramCols = 32
spectrogramsPerSequence = 1  # for post processing time filtering
num_epochs = 20

# 3 classes : 0 indoor, 1 outdoor, 2 in vehicle
ascLabels = {
            'indoor' : 0,
            'outdoor' : 1,
            'in-vehicle' : 2,
}

nclasses = len(ascLabels)
#Label Font Size for plots
labFontSize = 'xx-small' if (nclasses == 15) else 'medium'


#####         Data Set Loading and Feature Scaler Application           #######

print ('Loading : trainX and TrainY and Z-Score Scaler...\n')
X_train = joblib.load(featureSetsPath + 'X_training_noScale.pkl')
y_train = joblib.load(featureSetsPath + 'Y_training.pkl')

# Load Scaler and apply to test Sets
scaler  = joblib.load(featureSetsPath + 'zscore_scaler.pkl')

print ('Loading : Validation_X and validation_y ...\n')
X_val = joblib.load(featureSetsPath + 'X_validation_noScale.pkl')
Y_val = joblib.load(featureSetsPath + 'Y_validation.pkl')


print ('Loading: test_X and test_y ...\n')
X_test = joblib.load(featureSetsPath + 'X_test_noScale.pkl')
Y_test = joblib.load(featureSetsPath + 'Y_test.pkl')

#########        Scaler application to Train Validation and Test Sets        
X_train_r = X_train.reshape(X_train.shape[0],spectrogramRows*spectrogramCols)
X_train_r_scaled = scaler.transform(X_train_r)
X_train = X_train_r_scaled.reshape(X_train.shape[0],spectrogramRows,spectrogramCols,1) #check togliere 1 finale

X_val_r = X_val.reshape(X_val.shape[0],spectrogramRows*spectrogramCols)
X_val_r_scaled = scaler.transform(X_val_r)
X_val = X_val_r_scaled.reshape(X_val.shape[0],spectrogramRows,spectrogramCols,1)

X_test_r = X_test.reshape(X_test.shape[0],spectrogramRows*spectrogramCols)
X_test_r_scaled = scaler.transform(X_test_r)
X_test = X_test_r_scaled.reshape(X_test.shape[0],spectrogramRows,spectrogramCols,1)


#delete redundant vectors
del(X_train_r)
del(X_train_r_scaled)
del(X_val_r)
del(X_val_r_scaled)
del(X_test_r)
del(X_test_r_scaled)


#%%
##########           CNN Keras Model Build                  ###################
###############################################################################
print ('Building Model...')
## Initialization for Model Reproducibility
np.random.seed(1)
#tensorflow back end seeding

model = models.Sequential()
model.add(layers.Conv2D(16, (3, 3), activation='relu',input_shape=(30, 32, 1), data_format='channels_last'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(16, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Flatten())
model.add(layers.Dense(9, activation='relu'))
model.add(layers.Dense(nclasses, activation='softmax'))
model.summary()

#Model configuration for training 
sgd = optimizers.SGD(lr=0.01, momentum=0.9, nesterov=True)
model.compile(optimizer=sgd, loss='categorical_crossentropy',  metrics=['acc'])

#Labels Categorization
y_train = to_categorical(y_train, nclasses)
Y_val = to_categorical(Y_val, nclasses)
Y_test_cat = to_categorical(Y_test, nclasses)


#%%
################           Model Training               #######################
###############################################################################

history = model.fit(X_train, y_train, validation_data = (X_val, Y_val), batch_size=500, epochs=num_epochs, verbose=2)

############             Plot and Save Learning curves       ##################
loss = history.history['loss']
val_loss = history.history['val_loss']

plt.figure()
plt.clf()
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.plot(loss, color='r')
plt.plot(val_loss,color='g')
plt.legend(['training Loss', 'Validation Loss'], loc='upper left')
plt.savefig(logPath + 'Session_' + sessionNum + '_Loss' + '.png', dpi=200, format='png')
np.savetxt(logPath + 'Session_' + sessionNum + '_Training_Loss' + '.csv', loss, fmt='%.8f', delimiter=',', header = 'Training Loss')
np.savetxt(logPath + 'Session_' + sessionNum + '_Validation_Loss' + '.csv',val_loss, fmt='%.8f', delimiter=',', header = 'Validation Loss')

##############            Test set Evaluation               ###################
score = model.evaluate(X_test, Y_test_cat, verbose=2)
print('Test loss:', score[0])
print('Test accuracy:', score[1])


#%%
#####        Test Set Prediction - confusion Matrix on sequence base       ###

test_set_pred = model.predict(X_test)
print('Saving Model Prediction Outputs for Test Set: ......\n\n')
joblib.dump(test_set_pred, logPath + 'Session_' + sessionNum + '_Test_Set_predictions' + '.pkl', compress=5)


#Time filtering for the test Set on a sequence base (30")
Y_test_f = Y_test[::spectrogramsPerSequence]
test_set_pred_f = np.array([],dtype='int64')
#Time filtering for the test Set
for i in range(0,test_set_pred.shape[0],spectrogramsPerSequence):
    sequence_pred = test_set_pred[i:i+spectrogramsPerSequence].mean(axis=0).argmax()
    test_set_pred_f = np.append(test_set_pred_f, sequence_pred)

# Build and save confusion matrix on Test Set
test_conf_mtx = confusion_matrix(Y_test_f, test_set_pred_f)
np.savetxt(logPath + 'Session_' + sessionNum + '_Test_Set_confMatrix' + '.csv',test_conf_mtx, fmt='%7d', delimiter=',', header = 'Session_' + sessionNum + '_test Confusion Matrix')

# Calculate confusion matrix percentages
test_confMtx_perc = test_conf_mtx.astype('float') / test_conf_mtx.sum(axis=1)[:, np.newaxis] * 100.0

# Confusion Matrix Plotting and Saving
fig = plt.figure()
plt.clf()
ax = fig.add_subplot(111)
ax.set_aspect(1)
ax.get_yaxis().set_tick_params(direction='out')
ax.get_xaxis().set_tick_params(direction='out')
res = ax.imshow(np.array(test_confMtx_perc), cmap=plt.cm.YlGnBu_r, interpolation='nearest')

for x in range(nclasses):
    for y in range(nclasses):
        annotation = str((test_confMtx_perc[x][y])).split('.')[0] + '.' + str((test_confMtx_perc[x][y])).split('.')[1][0]
        ax.annotate(annotation, xy=(y,x), fontsize=labFontSize, color='#7f7f7f', horizontalalignment='center', verticalalignment='center')

plt.title('Test Set Confusion Matrix %')
plt.xticks(range(nclasses), sorted(ascLabels, key=ascLabels.get), rotation=90)
plt.yticks(range(nclasses), sorted(ascLabels, key=ascLabels.get))
plt.tight_layout()
plt.ylabel('True label')
plt.xlabel('Predicted label')
plt.tight_layout()
plt.savefig(logPath + 'Session_' + sessionNum + '_Test_Set_confMatrix' + '.png', dpi=200, format='png')

#%%
#########       Model Summary Saving (text format)                 ############

# Check for Python 2.7 it generates an error
with open(logPath + 'Session_' + sessionNum + '_Model_Summary' + '.txt','w') as fh:
    # Pass the file handle in as a lambda function to make it callable
    model.summary(print_fn=lambda x: fh.write(x + '\n'))

######                Keras Model Saving (HDF5 format)                 ########
model.save(logPath + 'Session_' + sessionNum + '_Model' + '.h5')
