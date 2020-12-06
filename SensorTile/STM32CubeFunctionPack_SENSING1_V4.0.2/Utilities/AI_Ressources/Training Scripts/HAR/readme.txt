This script has following dependencies

numpy, argparse, os, logging, warnings, datetime, pandas, scipy, matplotlib, mpl_toolkits.axes_grid1, sklearn, keras, tensorflow, tqdm

# private libraries
PrepareDataset, HARNN, preprocess, Utilities

Also note that when running from Jupyter notebook you also need "keras_tqdm".

Finally user must note that we are not providing any dataset with the function pack and user must download the dataset on his own. The WISDM dataset can be easily found and downloaded from http://www.cis.fordham.edu/wisdm/dataset.php. This dataset is to be downloaded and placed at following path with given name to make the script functional : 'datasets/WISDM_ar_v1.1_raw.txt'.
AST is a proprietary dataset of STMicroelectronics, hence not provided with the function pack. A dummy example of the dataset is provided in the dataset directory as two separate *.pkl file for train and test data. 


To make sure you have all the required packages with right versions run following command.
>> pip install requirements.txt