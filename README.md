# Text Classification with Multinomial Naive Bayes
In this project, we implement a text classifier using multinomial naive bayes
model on Reuters-21578 dataset. This is done in the following steps:

1. Construct training and test datasets using LEWISSPLIT tag.
2. Build naive bayes model.
  1. Normally, multinomial naive bayes uses all the words in the given
  documents. This generally results is good representations.
  2. Additionally, we implement mutual information feature selection to use only
  the top K most important words in training documents.
3. Predict the classes of test documents using the already built model.
4. Output metrics. In this project, we use micro-averaged, macro-averaged and
unaveraged precision, recall and F1-scores.

## Requirements
1. g++-5 and above with full C++14 support
2. cmake 3.2.2 and above
3. dirent.h header to retrieve file information. This normally comes installed
C POSIX library. However, if you are on Windows and compiling with MSVC,
you need to obtain this header manually.
4. Porter Stemmer in file src/porter\_stemmer.cpp. For reference page, go to
[https://tartarus.org/martin/PorterStemmer/](https://tartarus.org/martin/PorterStemmer/).
5. doxygen (optional)

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

To obtain the Reutrs-21578 dataset, please go to
[reuters21578](http://www.daviddlewis.com/resources/testcollections/reuters21578/),
download the gzipped tar archive and copy it into the project root directory.
Then, by running extract\_dataset.sh script, you can create the necessary
dataset layout.

Afterwards to to construct the datasets, simply run
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
./classifier --fit train.txt model.txt --num-features 50 2> important_words
```

This command will fit the classifier and output the most 50 important word for
each class to important\_words file.

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
