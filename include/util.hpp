#pragma once

#include <string>
#include <vector>

namespace ir {

/**
 * @brief Split the given input string using one of the delimeters and return
 * a vector of tokens.
 *
 * This function is a wrapper around well-known C strtok function to tokenize
 * a string using a list of delimiters.
 *
 * @param str String to tokenize. Characters that are one of the given
 * delimiters will be replaced by the NULL character.
 * @param delimiters List of delimiters.
 *
 * @return std::vector of resulting tokens.
 */
std::vector<std::string> split(std::string& str, const std::string& delimiters);

/**
 * @brief Check if the given value is one of the values in the given sequence.
 *
 * This function checks whether value is equal to one of the values in the
 * range [begin, end).
 *
 * @tparam T Equality-comparable type. (operator==)
 * @tparam InputIterator Iterator type whose value_type must be equality
 * comparable with T.
 * @param begin Beginning of the sequence [begin, end).
 * @param end End of the sequence [begin, end).
 * @param value Value to check if it is equal to one of the values in [begin,
 * end).
 *
 * @return true if value is one of the values in [begin, end); false, otherwise.
 */
template <typename T, typename InputIterator>
bool one_of(InputIterator begin, InputIterator end, const T& value) {
    for (auto it = begin; it != end; ++it) {
        if (*it == value) {
            return true;
        }
    }
    return false;
};

/**
 * @brief Intersect two sorted sequences in linear time using the merge
 * algorithm.
 *
 * This function finds the intersection of the given sorted sequences in linear
 * time by traversing the two sequences simultaneously. The intersection set
 * is written to the OutputIterator out.
 *
 * @tparam InputIterator1 Iterator type of the first sequence. Elements of the
 * two sorted sequences must be == and < comparable.
 * @tparam InputIterator2 Iterator type of the second sequence. Elements of the
 * two sorted sequences must be == and < comparable.
 * @tparam OutputIterator Iterator type of the output sequence.
 * @param first_begin Beginning of the first sequence.
 * @param first_end End of the first sequence.
 * @param second_begin Beginning of the second sequence.
 * @param second_end End of the second sequence.
 * @param out Beginning of the output sequence.
 *
 * @return Iterator pointing to the end of the output sequence.
 */
template <typename InputIterator1, typename InputIterator2,
          typename OutputIterator>
OutputIterator intersect(InputIterator1 first_begin, InputIterator1 first_end,
                         InputIterator2 second_begin, InputIterator2 second_end,
                         OutputIterator out) {
    // classical merge algorithm
    while (first_begin != first_end && second_begin != second_end) {
        if (*first_begin == *second_begin) {
            *out = *first_begin;
            ++first_begin;
            ++second_begin;
            ++out;
        } else if (*first_begin < *second_begin) {
            ++first_begin;
        } else {
            ++second_begin;
        }
    }
    return out;
};
} // namespace ir