#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Main namespace for all the functions and classes.
 */
namespace ir {
/**
 * @brief Typedef for a raw document.
 *
 * Documents that have not been processed at all are considered raw documents.
 */
using raw_doc = std::string;

/**
 * @brief Enum denoting if a document is a train or test document.
 */
enum class DocType {
    /**
     * @brief Train document.
     */
    Train,
    /**
     * @brief Test document.
     */
    Test,
    /**
     * @brief Other type of document that won't be used in training/testing.
     */
    Other
};

/**
 * @brief Enum denoting the class of a document.
 */
enum class DocClass { Earn, Acq, MoneyFx, Grain, Crude, Other };

/**
 * @brief Output operator for ir::DocClass.
 *
 * @param os Output stream to output the string representation of ir::DocClass.
 * @param doc_class ir::DocClass enum.
 *
 * @return Modified output stream.
 */
std::ostream& operator<<(std::ostream& os, DocClass doc_class);

/**
 * @brief Input operator for ir::DocClass.
 *
 * @param is Input stream to read the string representation of ir::DocClass.
 * @param doc_class ir::DocClass enum.
 *
 * @return Modified input stream.
 */
std::istream& operator>>(std::istream& is, DocClass& doc_class);

/**
 * @brief Typedef for an index that holds the id of a document and its raw
 * content. (train/test) and its raw content.
 */
using raw_doc_index = std::unordered_map<size_t, raw_doc>;

/**
 * @brief Typedef for an index that holds the id of a document and its type
 * as ir::DocType.
 */
using doc_type_index = std::unordered_map<size_t, DocType>;

/**
 * @brief Typedef for an index that holds the id of a document and its class.
 */
using doc_class_index = std::unordered_map<size_t, DocClass>;

/**
 * @brief Typedef for an index that holds the id of a document and vector of
 * the classes it belongs to.
 */
using doc_multiclass_index = std::unordered_map<size_t, std::vector<DocClass>>;

/**
 * @brief sample type to be used that requires words and their counts
 * as a sample.
 */
template <typename Word> using sample = std::unordered_map<Word, size_t>;

/**
 * @brief Representation of a single document as a classifier sample.
 */
using doc_sample = sample<std::string>;

/**
 * @brief Typedef for an index from the id of a document to a vector containing
 * the terms and their counts.
 *
 * A term is defined as a normalized token whereas a token is a maximum possible
 * sequence of non-whitespace characters. According to this definition, a token
 * may contain punctuation characters, as well. Depending on the normalization
 * operations, a term may or may not contain punctuation characters.
 */
using doc_term_index = std::unordered_map<size_t, doc_sample>;
} // namespace ir
