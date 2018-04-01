#pragma once

#include "defs.hpp"
#include <string>
#include <vector>

namespace ir {

/**
 * @brief Relative path from executable to dataset directory containing Reuters
 * files.
 */
const std::string DATASET_DIR = "Dataset";
/**
 * @brief Relative path from executable to text file containing stopwords.
 */
const std::string STOPWORD_PATH = "stopwords.txt";

/**
 * @brief Relative path from executable to the output training data.
 */
const std::string TRAIN_SET_PATH = "train.txt";

/**
 * @brief Relative path from executable to the output test data.
 */
const std::string TEST_SET_PATH = "test.txt";

/**
 * @brief Return a list of filepaths of unzipped Reuters data files under
 * ir::DATASET_DIR.
 *
 * This function assumes that all the Reuters datafiles are under
 * ir::DATASET_DIR.
 *
 * UNIX C-API is used to get filenames (specifically dirent.h)
 *
 * @return vector of strings where each string is the relative path from the
 * executable to one of 22 Reuters sgm files.
 */
std::vector<std::string> get_data_file_list();

/**
 * @brief Write a dataset to the given output stream.
 *
 * This function writes the dataset stored in term_index and class_index to the
 * given output stream os. The format of the written dataset is as follows:
 *
 * <blockquote>
 *     <DOC_ID> <DOC_CLASS>\n
 *     <WORD_1> <COUNT_1>\n
 *     <WORD_2> <COUNT_2>\n
 *     \f$\vdots\f$\n
 *     <WORD_N> <COUNT_N>\n
 *     \n
 *     <DOC_ID> <DOC_CLASS>\n
 *     <WORD_1> <COUNT_1>\n
 *     <WORD_2> <COUNT_2>\n
 *     \f$\vdots\f$\n
 *     <WORD_N> <COUNT_N>\n
 *     \f$\vdots\f$\n
 * </blockquote>
 *
 * @param os Output stream object to write the dataset to.
 * @param term_index Mapping from document id to words and their counts.
 * @param class_index Mapping from document id to class of the document.
 *
 * @return Modified output stream.
 */
std::ostream& write_dataset(std::ostream& os, const doc_term_index& term_index,
                            const doc_class_index& class_index);

/**
 * @brief Read a dataset from the given input stream.
 *
 * This function reads the dataset in the given input stream and returns a
 * ir::doc_term_index and ir::doc_class_index that stores the read dataset.
 * The dataset to read must be in the format as specified in ir::write_dataset.
 *
 * @param is Input stream from which the dataset will be read.
 * @return pair of ir::doc_term_index and ir::doc_class_index.
 */
std::pair<ir::doc_term_index, ir::doc_class_index>
read_dataset(std::istream& is);
} // namespace ir
