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
 *
 * @param os
 * @param index
 * @return
 */
std::ostream& write_dataset(std::ostream& os, const doc_term_index& term_index,
                            const doc_class_index& class_index);


std::pair<ir::doc_term_index, ir::doc_class_index>
read_dataset(std::istream& is);
} // namespace ir
