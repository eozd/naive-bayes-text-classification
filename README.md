# CmpE 493 Assignment 2

## Requirements
1. g++-5 and above with full C++14 support
2. cmake 3.2.2 and above
3. doxygen (optional)

## Build
To build the project, run the following commands in the project root directory

```
chmod +x build.sh
./build.sh release
```

This will build the project and create two executables: construct\_datasets and
classifier.

### Build Options
You can build the project in debug mode if you want to debug its execution trace
by running
```
./build.sh debug
```

If you want to get rid of all build files, run
```
./build.sh clean
```

## Documentation
You can view the documentation in the source files. Optionally, you can build
the project documentation using doxygen and view it in your browser. To build the
documentation, doxygen must be installed on your system. After installing doxygen,
type the following command in the project root directory to build the documentation:

```
./build.sh doc
```

To view the documentation, open ```doc/html/index.html``` file with your
browser.

## Running
The build process creates two executables:

### construct\_datasets
This is the executable to construct training and test datasets from Reuters
files.

construct\_datasets executable expects the following directory layout:

```
construct_datasets
stopwords.txt
Dataset
├── reut2-000.sgm
├── reut2-001.sgm
├── reut2-002.sgm
...
```

To construct the datasets, simply run
```
./construct_datasets
```

construct\_datasets creates a training set called train.txt and a test set
called test.txt

### classifier
classifier is the executable to train a Naive Bayes model or predict using an
already trained model. To see help message explaining program arguments, run

```
./classifier
```

classifier executable works by saving the trained model on a model file. To
predict, it requires an already predicted model file.

#### Training
To train a Naive Bayes classifier from a training set called train.txt run

```
./classifier --fit train.txt model.txt
```

The program will create model.txt file containing prior and likelihood counts
of each term-class pair.

You can also train a model using only a subset of the features. This subset is
chosen using mutual information criterion. To train a model by using the 50
most important word for each class, run

```
./classifier --fit train.txt model.txt --num-features 50
```

#### Predicting
To predict classes of all samples in a test set saved in test.txt
using an already trained and saved model in model.txt file run

```
./classifier --predict test.txt model.txt > out 2> log
```

After this program exits, out file will contain the actual and predicted label
of each class. Each line of out corresponds to a sample. log file will contain
metrics of the prediction. Micro averaged, macro averaged and unaveraged
precision, recall and F1-score metrics are saved to log.

##### Example out
```
ID: 15273 | Test:      grain | Pred:      grain
ID: 18482 | Test:      grain | Pred:      grain
```
