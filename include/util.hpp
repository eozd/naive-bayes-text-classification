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

template <typename Scalar>
double laplace_smooth(Scalar nom, Scalar denom, size_t total_items,
                      double alpha = 1) {
    return static_cast<double>(nom + alpha) / (denom + alpha * total_items);
}
} // namespace ir