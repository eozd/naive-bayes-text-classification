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
 * @brief Return Laplace smoothed version of the given fraction.
 *
 * This function returns the Laplace smoothed version of the given fraction in
 * nom and denom parameters. Laplace smooth with inputs \f$x, y\f$ and
 * parameters \f$\alpha, d\f$ is defined as follows:
 *
 * \f[
 * \mathcal{L}(x, y; \alpha, d) = \frac{x + \alpha}{y + d\alpha}
 * \f]
 *
 * @tparam Scalar1 Scalar type that can be summed with a double.
 * @tparam Scalar2 Scalar type that can be summed with a double.
 *
 * @param nom Nominator of the fraction (\f$x\f$).
 * @param denom Denominator of the fraction (\f$y\f$).
 * @param total_items Total number of items in smoothing (\f$d\f$).
 * @param alpha Smoothing parameter (\f$\alpha\f$).
 *
 * @return Laplace smoothed fraction.
 */
template <typename Scalar1, typename Scalar2>
double laplace_smooth(Scalar1 nom, Scalar2 denom, size_t total_items,
                      double alpha = 1) {
    return static_cast<double>(nom + alpha) / (denom + alpha * total_items);
}
} // namespace ir