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
 * @brief Typedef for an index that holds the id of a document and its raw
 * content.
 */
using raw_doc_index = std::unordered_map<size_t, raw_doc>;

/**
 * @brief Typedef for an index from the id of a document to a vector containing
 * the terms and their positions in that document.
 *
 * A term is defined as a normalized token whereas a token is a maximum possible
 * sequence of non-whitespace characters. According to this definition, a token
 * may contain punctuation characters, as well. Depending on the normalization
 * operations, a term may or may not contain punctuation characters.
 */
using doc_term_index =
    std::unordered_map<size_t, std::vector<std::pair<std::string, size_t>>>;

/**
 * @brief Typedef for a data structure representing the positional inverted
 * index of a document corpus.
 *
 * In positional inverted index, each term (std::string) is mapped to a vector
 * of pairs. The first element of a pair is the document id. The second element
 * of a pair is a vector of integers, the positions where the term occurs in
 * this particular document.
 */
using pos_inv_index =
    std::unordered_map<std::string,
                       std::vector<std::pair<size_t, std::vector<size_t>>>>;

/**
 * @brief Typedef for a data structure mapping a term to a unique ID.
 */
using term_id_map = std::unordered_map<std::string, size_t>;

/**
 * @brief Typedef for a data structure mapping ID of a term to vector of
 * <doc_id, pos_vec> pairs.
 *
 * Each term ID is mapped to a vector of pairs. Each pair contains the ID of a
 * document containing this term. The second element of each pair contains the
 * vector of positions where the term occurs in the document.
 */
using id_pos_map =
    std::unordered_map<size_t,
                       std::vector<std::pair<size_t, std::vector<size_t>>>>;
} // namespace ir
