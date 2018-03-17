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
        count = {{{0, 0}, {0, 0}}};
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
} // namespace ir
