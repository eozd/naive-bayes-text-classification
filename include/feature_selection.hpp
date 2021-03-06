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
#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include <vector>

namespace ir {
/**
 * @brief Compute Mutual Information between every word and the given target
 * class.
 *
 * This function computes the Mutual Information between each word \f$w\f$ and
 * the given target class \f$c\f$ using the formula:
 *
 * \f[
 *     \sum_{e_w}\sum_{e_c}p(e_w,e_c)\log{\frac{p(e_w,e_c)}{p(e_w)p(e_c)}}
 * \f]
 *
 * where \f$e_w \in \{0,1\}\f$ denotes whether word \f$w\f$ occurs in document
 * \f$d\f$ and \f$e_c \in \{0,1\}\f$ denotes whether that document belongs to
 * class \f$c\f$.
 *
 * @tparam Word Type of words that occur in documents. For text documents, this
 * is generally a variant of std::string.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param x vector of sample documents (training data).
 * @param y vector of corresponding sample classes
 * @param target Target class with respect to which the mutual info of every
 * word will be computed.
 *
 * @return Mapping that contains the mutual information between every word
 * in the given sample set and the target class
 */
template <typename Word, typename Class>
std::unordered_map<Word, double> mutual_info(const std::vector<sample<Word>>& x,
                                             const std::vector<Class>& y,
                                             const Class& target) {
    assert(x.size() == y.size());
    const size_t n_samples = x.size();

    // create the dictionary of all words
    std::set<Word> dict;
    for (const auto& smp : x) {
        for (const auto& pair : smp) {
            dict.insert(pair.first);
        }
    }

    // index from each word to document IDs (position) it appears in parameter x
    std::unordered_map<Word, std::vector<size_t>> word_docs;
    for (size_t i = 0; i < n_samples; ++i) {
        for (const auto& pair : x[i]) {
            const Word& word = pair.first;
            word_docs[word].push_back(i);
        }
    }

    // number of docs belonging to class target
    auto n_target_cls =
        static_cast<size_t>(std::count(y.begin(), y.end(), target));

    // result
    std::unordered_map<Word, double> mut_info_map;

    constexpr size_t n_rows = 2;
    constexpr size_t n_cols = 2;
    // table of counts ([0][0] <--> e_w = 0 AND e_c = 0, and so on)
    std::array<std::array<size_t, n_cols>, n_rows> count;
    for (const Word& word : dict) {
        // set all counts to 0
        std::for_each(count.begin(), count.end(),
                      [](auto& arr) { arr.fill(0); });

        for (size_t id : word_docs[word]) {
            if (y[id] == target) {
                ++count[1][1];
            } else {
                ++count[1][0];
            }
        }
        // fill using normalization constraints
        count[0][1] = n_target_cls - count[1][1];
        count[0][0] = (n_samples - n_target_cls) - count[1][0];

        const size_t total =
            count[0][0] + count[0][1] + count[1][0] + count[1][1];
        const auto total_d = static_cast<double>(total);
        double mut_info = 0;
        for (size_t i = 0; i < n_rows; ++i) {
            for (size_t j = 0; j < n_cols; ++j) {
                if (count[i][j] == 0) {
                    // assume 0log0 = 0
                    continue;
                }
                size_t row_sum = count[i][0] + count[i][1];
                size_t col_sum = count[0][j] + count[1][j];
                size_t nom = total * count[i][j];
                size_t denom = row_sum * col_sum;

                mut_info += ((count[i][j] * std::log2(nom)) -
                             (count[i][j] * std::log2(denom))) /
                            total_d;
            }
        }

        mut_info_map[word] = mut_info;
    }

    return mut_info_map;
};

/**
 * @brief Get top K important word for each class using Mutual Information.
 *
 * This function finds the top K most important word for each class and
 * returns a map from each class to a vector of most important words. Important
 * words for each class are found using ir::mutual_info function.
 *
 * @tparam Word Type of words that occur in documents. For text documents, this
 * is generally a variant of std::string.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param x_train vector of sample documents (training data).
 * @param y_train vector of corresponding sample classes
 * @param class_dict Set containing all the classes in the given training set.
 * @param top_k Number of most important words
 *
 * @return Map from class to a vector of corresponding most important words.
 *
 * @remark Time complexity is \f$\Theta(CN + CK\log{N})\f$ where \f$C\f$ is
 * the number of classes, \f$N\f$ is the total number of words, \f$K\f$ is the
 * number of most important words.
 */
template <typename Word, typename Class>
ir::unordered_enum_map <Class, std::vector<Word>>
get_top_words_per_class(const std::vector<sample<Word>>& x_train,
                        const std::vector<Class>& y_train,
                        const std::set<Class>& class_dict, const size_t top_k) {
    auto max_lambda = [](const auto& left, const auto& right) {
        return left.second < right.second;
    };

    ir::unordered_enum_map <Class, std::vector<Word>> top_words_per_class;
    // find important words per class
    for (const Class& doc_class : class_dict) {
        // map from all words to their mut info values
        auto mut_info_map = ir::mutual_info(x_train, y_train, doc_class);

        // make a heap in linear time
        std::vector<std::pair<Word, double>> mut_info_vec;
        std::copy(mut_info_map.begin(), mut_info_map.end(),
                  std::back_inserter(mut_info_vec));
        std::make_heap(mut_info_vec.begin(), mut_info_vec.end(), max_lambda);

        // get top K words in KlogN time
        std::vector<Word> top_k_words;
        for (size_t i = 0; i < top_k; ++i) {
            top_k_words.push_back(mut_info_vec.front().first);
            std::pop_heap(mut_info_vec.begin(), mut_info_vec.end(), max_lambda);
            mut_info_vec.pop_back();
        }
        top_words_per_class[doc_class] = top_k_words;
    }

    return top_words_per_class;
};

/**
 * @brief Remove unimportant words from the training set using a mapping from
 * each class to most important words for that class.
 *
 * For a document \f$D\f$ that belongs to class \f$C\f$ and a set of important
 * words for class \f$C\f$ called \f$V_C\f$, this function removes all words
 * from document \f$D\f$ that is not in \f$V_C\f$. To do this efficiently,
 * it is required that \f$V_C\f$ must be sorted.
 *
 * @tparam Word Type of words that occur in documents. For text documents, this
 * is generally a variant of std::string.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param x_train vector of sample documents (training data).
 * @param y_train vector of corresponding sample classes
 * @param top_words_per_class Mapping from each class to a vector of most
 * important words. Vector of most important word must be sorted.
 *
 * @remark Time complexity is \f$\Theta(N_1\log{K_1} + N_2\log{K_2} + \dots +
 * N_C\log{K_C})\f$ where \f$N_i\f$ is the total number of words in all
 * documents belonging to class \f$i\f$, \f$K_i\f$ is the number of most
 * important words for class \f$i\f$.
 */
template <typename Word, typename Class>
void remove_unimportant_words(
    std::vector<sample<Word>>& x_train, std::vector<Class>& y_train,
    const ir::unordered_enum_map <Class, std::vector<Word>>& top_words_per_class) {

    // for each class
    for (const auto& pair : top_words_per_class) {
        const Class& cls = pair.first;
        const auto& top_words = pair.second;

        // for each doc
        for (size_t i = 0; i < y_train.size(); ++i) {
            // only for docs in current class
            if (y_train[i] != cls) {
                continue;
            }

            // words in current doc
            std::vector<Word> words;
            for (const auto& word_count_pair : x_train[i]) {
                words.push_back(word_count_pair.first);
            }

            // if word is not in important vector, remove it
            for (const auto& word : words) {
                bool not_top_word = !std::binary_search(top_words.begin(),
                                                        top_words.end(), word);
                if (not_top_word) {
                    x_train[i].erase(word);
                }
            }
        }
    }
};
} // namespace ir
