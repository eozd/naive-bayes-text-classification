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
 * @brief Relative path from executable to the dictionary built by indexer and
 * used by searcher.
 */
const std::string DICT_PATH = "dict.txt";
/**
 * @brief Relative path from executable to the positional inverted index built
 * by indexer and used by searcher.
 */
const std::string INDEX_PATH = "index.txt";
/**
 * @brief Tag to use to denote posting list sequence has started in the index
 * file.
 */
const std::string POS_LIST_BEG_TAG = "<POS_LIST>";
/**
 * @brief Tag to use to denote posting list sequence has enden in the index
 * file.
 */
const std::string POS_LIST_END_TAG = "</POS_LIST>";

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
 * @brief Form a map of <term : ID> and write it to a file at ir::DICT_PATH.
 *
 * Each line of ir::DICT_PATH is of the form
 *
 * <blockquote>
 *
 * \<TERM\> : <ID>
 *
 * </blockquote>
 *
 * where \<TERM\> is a term in the given inverted index and <ID> is a unique ID
 * assigned to \<TERM\>.
 *
 * @section example Example entries
 *
 * <blockquote>
 * hfl 31932\n
 * t-bond 31933\n
 * miyaji 31934\n
 * micropro 31935\n
 * </blockquote>
 *
 * @param inverted_index Positional inverted index constructed from the corpus.
 */
void write_dict_file(const pos_inv_index& inverted_index);

/**
 * @brief Form a map of <ID : list<ID, list<pos>>> and write the resulting index
 * to a file at ir::INDEX_PATH.
 *
 * An entry in ir::INDEX_PATH is of the form
 *
 * <blockquote>
 *
 * <ID>\n
 * <BEGIN_TAG>\n
 * <TAB><DOC_ID> : <POS> <POS> \f$\dots\f$ <POS>\n
 * <TAB><DOC_ID> : <POS> <POS> \f$\dots\f$ <POS>\n
 * \f$\vdots\f$\n
 * <TAB><DOC_ID> : <POS> <POS> \f$\dots\f$ <POS>\n
 * <END_TAG>\n
 *
 * </blockquote>
 *
 * @section example Example entry
 *
 * <blockquote>
 *
 * 63856\n
 * <POS_LIST>\n
 *     18501 : 53 65 79\n
 *     19512 : 82\n
 * </POS_LIST>\n
 *
 * </blockquote>
 *
 * @param inverted_index
 */
void write_index_file(const pos_inv_index& inverted_index);

/**
 * @brief Read the dictionary at ir::DICT_PATH and return it.
 *
 * Dictionary file is parsed according to the specification given in the
 * documentation of write_dict_file.
 *
 * @return Dictionary from term to IDs.
 */
term_id_map read_dict_file();

/**
 * @brief Read the index at ir::INDEX_PATH and return it.
 *
 * Index file is parsed according to the specification given in the
 * documentation of write_index_file.
 *
 * @return Index from term ID to a vector of document IDs and positions in
 * documents.
 */
id_pos_map read_index_file();
} // namespace ir
