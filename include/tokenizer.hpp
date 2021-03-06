/*
 * Copyright 2018 Esref Ozdemir
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defs.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace ir {

/**
 * @brief Class to handle tokenization and normalization operations and keep
 * statistics about these operations.
 */
class Tokenizer {
  public:
    /**
     * @brief Split the given string with respect to whitespace characters and
     * return the resulting tokens and their positions in the document as a
     * vector.
     *
     * @param str Input string to tokenize.
     *
     * @return std::vector of pairs containing the tokens and their positions.
     */
    std::vector<std::string> tokenize(const std::string& str);

    /**
     * @brief Remove certain punctuation characters from certain parts of the
     * given string and return a copy.
     *
     * Punctuation characters are removed as specified in ir::normalize.
     *
     * @param token Input token from which punctuation will be removed.
     *
     * @return String with certain punctuation characters removed from token.
     */
    std::string remove_punctuation(const std::string& token);

    /**
     * @brief Tokenize and normalize a given raw document and return a
     * vector of terms and their counts.
     *
     * This function tokenizes the given raw document by splitting it by
     * whitespace, and then does normalization operations to each token as
     * defined in ir::normalize.
     *
     * @param doc Raw document.
     *
     * @return std::vector of normalized terms and their counts in the given raw
     * document.
     */
    ir::doc_sample get_doc_terms(const raw_doc& doc);

    /**
     * @brief Return the normalized version a given token.
     *
     * Token normalization consists of the following steps:
     *
     * 1. Punctuation removal
     *   * All punctuation characters at the beginning and at the end of the
     * token is removed. Additionally, <blockquote>' " < > </blockquote>
     * characters are removed from anywhere in the token.
     *
     * 2. Case folding
     *   * All characters are converted to lowercase.
     * 3. Stopword removal
     *   * If the given token is a stopword, an empty string is returned.
     * 4. Stemming
     *   * The resulting token is stemmed using the Porter Stemmer.
     *
     * @param token Token to normalize.
     *
     * @return Normalized version of the token. IF the given token is a
     * stopword, an empty string is returned.
     */
    std::string normalize(const std::string& token);

    /**
     * @brief Normalize all the tokens in the given vector of tokens
     * in-place.
     *
     * This function is a convenience function to apply ir::normalize to
     * each element of input token_vec and then remove empty strings from
     * the vector.
     *
     * @param token_vec vector of tokens to normalize in-place.
     */
    void normalize_all(std::vector<std::string>& token_vec);

    /**
     * @brief Check whether the input string is a stopword.
     *
     * This function simply checks if the given word is in the stopword list
     * defined in ir::STOPWORD_PATH.
     *
     * For efficiency purposes, the file is read only once when the function
     * is called for the first time, and the stopword list is sorted. Then,
     * stopword check is done using binary search.
     *
     * @param word Word to check if it is a stopword.
     *
     * @return true if word is in stopword list; false, otherwise.
     */
    bool is_stopword(const std::string& word);
};
} // namespace ir
