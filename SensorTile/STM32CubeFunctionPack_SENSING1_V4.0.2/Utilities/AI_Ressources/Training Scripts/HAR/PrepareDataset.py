#!/usr/bin/env python
# coding: utf-8 

#   This software component is licensed by ST under BSD 3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause
  

'''
Data preparations utilities for the Human Activity Recognition (HAR) demonstration
 
'''

# Import required dependencies and fix seeds for random generator for tensorflow and numpy for reproducibility
import numpy as np
np.random.seed(611)

# pandas to read and process the data tables
import pandas as pd 

# scipy for labeling the segments
from scipy import stats

# to one hot code the labels
from keras.utils import to_categorical

# library to generate filter for gravity rotation and preprocessing
from scipy.signal import butter, resample 
# gravity rotation dependency
from preprocess import gravity_rotation

# library to show plots
from matplotlib import pyplot as plt

# libraries for file handling
from os import listdir
from os.path import isfile, join

# to use a function to check if running from jupyter notebook or not
from Utilities import *

# to show the progress bar during segmenting the data
if is_notebook():
    from tqdm import tqdm_notebook as tqdm
else:
	from tqdm import tqdm

class DataHelper():
	def __init__( self, dataset, loggedDataDir, merge, modelName, seqLength, seqStep, preprocessing, trainTestSplit, trainValidSplit, resultDir ):
		self.dataset = dataset
		self.loggedDataDir = loggedDataDir
		self.merge = merge
		self.modelName = modelName
		self.seqLength = seqLength
		self.seqStep = seqStep
		self.preprocessing = preprocessing
		self.trainTestSplit = trainTestSplit
		self.trainValidSplit = trainValidSplit
		self.resultDir = resultDir
		# creating a list of activities available in two datasets
		# in case a new dataset has to be included with different classes thi
		if self.dataset == 'WISDM':
			if self.merge:
				self.classes = classes = [ 'Jogging', 'Stationary', 'Stairs', 'Walking' ]
			else:
				self.classes = [ 'Downstairs', 'Jogging', 'Sitting', 'Standing', 'Upstairs', 'Walking' ]
		elif self.dataset == 'AST':
			self.classes = [ 'Stationary', 'Walking', 'Jogging', 'Biking', 'Driving' ]
		else:
			# this will generate error as only two datasets are supported for now.
			self.classes = None


	def split_train_test_data( self, dataset ):
		# split train and test dataset with trainTestSplit proportion. (trainTestSplit out of 1 stay as train)
		datasetTrain = dataset[ 0 : int( dataset.shape[ 0 ] * self.trainTestSplit ) ]
		datasetTest = dataset[ int( dataset.shape[ 0 ] * self.trainTestSplit ) : int( dataset.shape[ 0 ] ) ]
		return datasetTrain, datasetTest

	def read_pkl_return_df( self, pklFilePath ):
		
		# initialize the script
		class_id = 0
		fileNr = 0
		myArray = [ ]

		# read pkl dataset
		Dataset = pd.read_pickle( pklFilePath )

		# list with nr files for every activity
		nrFilesPerClass = [ ]
		
		# we know there are only five activities in the dataset with labels from 0->4 so let us count nr of files for every activity
		for lbl in range( 5 ):
			nrFilesPerClass.append( Dataset[ 'act' ].count( lbl ) )
		
		# acceleration data in the dataset
		arrayData = np.array( Dataset[ 'acc' ] )

		# activity labels in dataset 
		arrayActivity = np.array( Dataset[ 'act' ] )
		
		# now let us get data for every activity one by one
		for nrFiles in nrFilesPerClass:
			# for every occurance of the label
			for i in range( fileNr, fileNr + nrFiles ):
				myArray.append( [ ] )
				for j in range( arrayData[ i ].shape[ 0 ] ):
					# for every sample in the file
					myArray[ i ].append( [ ] )
					myArray[ i ][ j ].extend( arrayData[ i ][ j ] )
					myArray[ i ][ j ].append( class_id )
			fileNr += nrFiles
			class_id += 1

		# preparing a vertical stack for the dataset 
		myArray = np.vstack( myArray[ : ] )

		# creating a dataframe to be consistent with WISDM data
		columns = [ 'x', 'y', 'z', 'Activity_Label' ]
		myDataset = pd.DataFrame( myArray, columns = columns )

		# replace activity code with activity labels to be consistent with WISDM dataset
		myDataset[ 'Activity_Label' ] = [ str( num ).replace( str( num ), self.classes[ int( num ) ] ) for num in myDataset[ 'Activity_Label' ] ]
		return myDataset		
	
	def read_dataset( self ):
		
		if self.dataset == 'WISDM':
			dataFilePath = 'datasets/WISDM_ar_v1.1_raw.txt'
			# read all the data in csv 'WISDM_ar_v1.1_raw.txt' into a dataframe called dataset
			columns = ['User', 'Activity_Label', 'Arrival_Time', 'x', 'y', 'z'] # headers for the columns
			dataset = pd.read_csv( dataFilePath, header=None, names=columns )

			# removing the ; at the end of each line and casting the last variable to datatype float from string
			dataset['z'] = [float(str(char).replace(";", "")) for char in dataset['z']]

			# remove the user column as we do not need it
			dataset = dataset.drop('User', axis=1)

			# as we are workign with numbers, let us replace all the empty columns entries with NaN (not a number)
			dataset.replace(to_replace='null', value = np.NaN)

			# remove any data entry which contains NaN as a member
			dataset = dataset.dropna( axis = 0, how = 'any' )
			if self.merge:
				dataset[ 'Activity_Label' ] = [ 'Stationary' if activity == 'Standing' or activity == 'Sitting' else activity for activity in dataset[ 'Activity_Label' ] ]
				dataset[ 'Activity_Label' ] = [ 'Stairs' if activity == 'Upstairs' or activity == 'Downstairs' else activity for activity in dataset[ 'Activity_Label' ] ]

			# removing the columns for time stamp and rearranging remaining columns to match AST dataset 
			dataset = dataset[ [ 'x', 'y', 'z', 'Activity_Label' ] ]
			datasetTrain, datasetTest = self.split_train_test_data( dataset )
			return datasetTrain, datasetTest
		
		elif self.dataset == 'AST':
			# note that the train and test data for AST are already seperate so we do not need to use split_train_test_data function
			trainDataFilePath = 'datasets/train.pkl'
			testDataFilePath =  'datasets/test.pkl'
			return self.read_pkl_return_df( trainDataFilePath ), self.read_pkl_return_df( testDataFilePath )
		else:
			raise NameError('Error: Please only choose one of the two datasets, WISDM or AST')
			return null

	def preprocess_data( self, data ):
	    
	    # choose a sample frequency
		if self.dataset == 'WISDM':
			fSample = 20
		else:
			fSample = 26

		if self.preprocessing:
			# create a copy of data to avoid overwriting the passed dataframe
			datacopy = data.copy()
			# create highpass filter to remove dc components
			fCut = 0.4
			# fSample = 20
			filterType = 'highpass'
			fNorm = fCut / fSample
			num,den = butter( 4, fNorm, btype = filterType )

			# preprocess the dataset by finding and rotating the gravity axis
			data_x = datacopy[ datacopy.columns[ : 3 ] ]
			datacopy[ datacopy.columns[ : 3 ] ] = gravity_rotation( np.array( data_x, dtype = float ), den, num )
			return datacopy
		else:
			return data

	def get_segment_indices( self, dataColumn ):

		# get segment indices to window the data into overlapping frames
		init = 0
		while init < len( dataColumn ):
			yield int( init ), int( init + self.seqLength )
			init = init + self.seqStep

	def get_data_segments( self, dataset ):

		# segmentng the data into overlaping frames
		dataIndices = dataset.index.tolist() 
		nSamples = len( dataIndices )

		segments = [ ]
		labels = np.empty( ( 0 ) )

		# need the following variable for tqdm to show the progress bar
		numSegments = int( np.floor( ( nSamples - self.seqLength ) / self.seqStep ) + 1 ) 

		# creating segments until the get_segment_indices keep on yielding the start and end of the segments
		for (init, end) in tqdm( self.get_segment_indices( dataIndices ), unit = ' segments', desc = 'Segments built ', total = numSegments + 1 ):
			
			# check if the nr of remaing samples are enough to create a frame
			if( end < nSamples ):
				segments.append( np.transpose( [ dataset[ 'x' ].values[ init : end ],
												 dataset[ 'y' ].values[ init : end ], 
												 dataset[ 'z' ].values[ init : end ] ] ) )
				
				# use the label which occured the most in the frame
				labels = np.append( labels, stats.mode( dataset[ 'Activity_Label' ][ init : end ] )[ 0 ][ 0 ] )
		
		# converting the segments from list to numpy array
		segments = np.asarray( segments, dtype = np.float )
		return segments, labels

	def reshape_sequences( self, sequences, labels ):
		# this function reshapes the sequences into a shape which is acceptable by tensorflow
		# i.e. channel_last
		# it also enocdes the class labels into one_hot_code
		# finally it splits the data into train and validation parts using trainValidSplit 
		# proportions suplied by the user

		
		# reshaping the sequences to generate tensors with channel last configuration
		reshaped_sequences = sequences.reshape( sequences.shape[ 0 ], sequences.shape[ 1 ], sequences.shape[ 2 ], 1 )
		
		# one hot code the labels
		labels = to_categorical( [ self.classes.index( label ) for label in labels ], num_classes = len(self.classes) )

		# splitting train and validation data
		train_test = np.random.rand( len( reshaped_sequences ) ) < self.trainValidSplit
		train_x = reshaped_sequences[ train_test ]
		train_y = labels[ train_test ]
		validation_x = reshaped_sequences[ ~train_test ]
		validation_y = labels[ ~train_test ]
		return train_x, train_y, validation_x, validation_y


	def prepare_data( self ):

		# this function prepares the dataset for training, validation and testing by generating
		# six tensors namely train_x, train_y, validation_x, validation_y, test_x, test_y

		# read public datasets
		datasetTrain, datasetTest = self.read_dataset( )

		# preprocess train and test datasets
		datasetTrainPreprocessed = self.preprocess_data( datasetTrain )
		datasetTestPreprocessed = self.preprocess_data( datasetTest )

		# segment train and test datasets
		print( 'Segmenting Train data' )
		( sequencesTrain, labelsTrain ) = self.get_data_segments( datasetTrainPreprocessed )

		print( 'Segmenting Test data' )
		( sequencesTest, labelsTest ) = self.get_data_segments( datasetTestPreprocessed )

		print( 'Segmentation finished!' )

		# reshape sequences
		train_x, train_y, validation_x, validation_y = self.reshape_sequences( sequencesTrain, labelsTrain )
		test_x, test_y, _ , _ = self.reshape_sequences( sequencesTest, labelsTest )

		# if self logged dataset is to be processed and used prepare the dataset and combine it with the big available datasets AST or WISDM
		if self.loggedDataDir != '':
			train_x1, train_y1, validation_x1, validation_y1, test_x1, test_y1 = self.prepare_self_logged_data()
			train_x = np.concatenate( ( train_x, train_x1 ), axis = 0)
			train_y = np.concatenate( ( train_y, train_y1 ), axis = 0)
			validation_x = np.concatenate( ( validation_x, validation_x1 ), axis = 0)
			validation_y = np.concatenate( ( validation_y, validation_y1 ), axis = 0)
			test_x = np.concatenate( ( test_x, test_x1 ), axis = 0)
			test_y = np.concatenate( ( test_y, test_y1 ), axis = 0)

		# return data tensors
		return train_x, train_y, validation_x, validation_y, test_x, test_y

	# get all file names in the provided data directory
	def get_file_names( self, fileType ):
		return [ f for f in listdir( self.loggedDataDir ) 
	         if ( isfile( join( self.loggedDataDir, f ) ) and ( f[ -4: ] == fileType ) ) ]

	def get_data_from_file( self, fileName, preparedDataFileName ):

		# get data from the given file
		filePath = join( self.loggedDataDir, fileName )
		dataset = pd.read_csv( filePath, header = 1 )
		eof = False
		seqEnd = -1
		labels = [ ]
		fileData = [ ]
		while eof != True :

			# find first valid index where label appears
			seqStart = dataset[dataset.columns[ 1 ] ][ seqEnd + 1 : ].first_valid_index( )
			
			# if it is a None, it means the end of file is reached without finding a label
			if( seqStart != None ):

				# otherwise find the index for label closing
				seqEnd = dataset[dataset.columns[ 1 ] ][ seqStart + 1 : ].first_valid_index()
				if seqEnd == None:

					# if the end is reached without finding the label closing take the data until last line
					seqEnd = len( dataset )
					eof = True

				else:
					# otherwise check if opening and closing labels are same if not then it is a faulty file reject it
					if( dataset[ dataset.columns[ 1 ] ] [ seqStart ] [ 1 : ] != dataset[ dataset.columns[ 1 ] ] [ seqEnd ] [ 1 : ] ):
						labels = [ ]
						fileData = [ ]
						break
				x_label = dataset[ dataset.columns[ 1 ] ] [ seqStart ] [ 1 : ]
				if not x_label in self.classes:
					# check if the file contains any label which are not present in self.classes
					# it is a bad file reject it 
					break
				# otherwise use the dataset
				x_data = np.asarray( dataset[ dataset.columns[ 2 : ] ] [ seqStart + 1 : seqEnd ], dtype = np.float )
				
				if self.dataset == 'WISDM':
					# WISDM has a sample rate of 20Hz while self generated AI logs from IOT boards are at 26Hz.
					# We need to resample the data to match WISDM sampling rate
					# Also WISDM has units which are m/s2, while IOT board has units mg (mili gravity), to be consistent
					# we need to convert the units.

					# resample to match the WISDM data
					x_data = resample( x_data, int( 20 / 26 * len( x_data ) ) )
					# changing to units from mg (mili gravity) to m/s2
					x_data = x_data / 1000 * 9.8
				
				# putting labels for x, y and z axis values
				x_dataset = pd.DataFrame( x_data, columns = [ 'x', 'y', 'z' ] )

				# adding activity label axis as 4th axis to be consistent with read_dataset function
				x_dataset.insert( 3, 'Activity_Label', x_label )

				# write the dataset into a CSV file and put it in result directory
				x_dataset.to_csv( preparedDataFileName, header = False, index = False, mode='a' )

				if x_label in labels :
					# label is already in labels list
					indx = labels.index( x_label )
					fileData[ indx ] = np.concatenate( ( fileData[ indx ], x_data ), axis = 0 )
				else:
					labels.append( x_label )
					fileData.append( np.asarray( x_data ) )
			else:
				eof = True

	def prepare_self_logged_data( self ):
		# this function prepares self logged data logged through STM32 board
		# we used L4 IOT boards
		print('preparing data file from all the files in directory ', self.loggedDataDir )

		# get the list of files in the directory
		filesInDirectory = self.get_file_names( fileType = '.csv' )

		# generating name for the resulting CSV file
		preparedDataFileName = join( self.resultDir, self.loggedDataDir.split( '/' )[ -1 ] + '.csv' )

		# going through list of files and after taking and preprocessing the data writing it to resulting CSV file
		for fileName in filesInDirectory:
		    print( 'parsing data from ', fileName )
		    self.get_data_from_file( fileName, preparedDataFileName )

		if isfile( preparedDataFileName ):
			# this means the data from AI logged files have been cleaned and dumped in preparedDatFileName file.
			# so let us work on this.
			columns = [ 'x', 'y', 'z', 'Activity_Label' ] # headers for the columns
			dataset = pd.read_csv( preparedDataFileName, header = None, names = columns )
			
			# split dataset into train and test data
			datasetTrain, datasetTest = self.split_train_test_data( dataset )

			# preprocess train and test dataset
			datasetTrainPreprocessed = self.preprocess_data( datasetTrain )
			datasetTestPreprocessed = self.preprocess_data( datasetTest )

			# segment train and test datasets
			print( 'Segmenting the AI logged Train data' )
			sequencesTrain, labelsTrain = self.get_data_segments( datasetTrainPreprocessed )

			print( 'Segmenting the AI logged Test data' )
			sequencesTest, labelsTest = self.get_data_segments( datasetTestPreprocessed )
			print( 'Segmentation finished!' )

			# reshape train and test datasets
			train_x, train_y, validation_x, validation_y = self.reshape_sequences( sequencesTrain, labelsTrain )
			test_x, test_y, _ , _  = self.reshape_sequences( sequencesTest, labelsTest )

			# return tensors made in the function
			return train_x, train_y, validation_x, validation_y, test_x, test_y

		else:
			print( 'Warning' )
			print( 'No data with required labels was found in the AI logged directory:' + self.loggedDataDir )
			return None

	def dump_data_for_post_validation( self, testX, testY, nrSamples ):
		# this function create data dumps from the prepared test data to later use for 
		# validating the converted code from CubeAI. This randomise the data provided and
		# save nrSamples for each of the classes with corresponding outputs into post_validation_data.npz file
		# with two variables x_test and y_test which is standard format for post validation data.
		
		# randomizing the order of the sequences
		p = np.random.permutation( len( testX ) )
		testX = testX[ p, :, :, : ]
		testY = testY[ p, : ]
		
		# generating empty arrays to save the x_test and y_test data
		x_test = np.empty( ( 0, testX.shape[ 1 ], testX.shape[ 2 ], testX.shape[ 3 ] ) )
		y_test = np.empty( ( 0, testY.shape[ 1 ] ) )

		# run a loop for all classes
		for i in range( testY.shape[ 1 ] ):

			# find two indices which for the current class
			inds = np.where( [ np.argmax( testY, axis = 1 ) == i ] )[ 1 ][ 0 : nrSamples ]

			# put the data into x_test and y_test variables
			x_test = np.concatenate( ( x_test, testX[ inds, :, :, : ] ), axis = 0  )
			y_test =  np.concatenate( ( y_test, testY[ inds, : ] ), axis = 0  )

			# save the data into a npz file.
		np.savez( join( self.resultDir, 'post_validation_data.npz' ), x_test, y_test )