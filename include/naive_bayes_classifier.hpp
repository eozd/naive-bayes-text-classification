#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>

#include "defs.hpp"

namespace ir {

/****************************** INTERFACE **********************************/

/**
 * @brief Template Multinomial Naive Bayes classifier that classifies documents
 * consisting of words and counts to given classes.
 *
 * @tparam Word Type of words that occur in documents. For text documents, this
 * is generally a variant of std::string.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 */
template <typename Word, typename Class> class NaiveBayesClassifier {
  public:
    /**
     * @brief Representation of a single document.
     *
     * Multinomial Naive Bayes requires words and their counts in the documents.
     */
    using sample = std::unordered_map<Word, size_t>;

    /**
     * @brief Representation of prior class distribution.
     *
     * Each entry of prior_t is a mapping from a class to its prior probability.
     */
    using prior_t = std::unordered_map<Class, double>;

    /**
     * @brief Representation of likelihood \f$p(w|c)\f$.
     *
     * Due to Multinomial Naive Bayes assumptions
     *
     * 1. Documents are bag of words
     * 2. Features (word positions) are conditionally independent given class
     *
     * joint likelihood \f$p(x_1, \dots, x_n | c_j)\f$ is factorized to product
     * of marginal likelihoods \f$p(x_1|c_j)p(x_2|c_j)\dotsp(x_n|c_j)\f$ where
     * for a given word \f$w\f$, \f$p(x_i|c_j) = p(x_k|c_j)\f$, i.e. word
     * positions are not important in determining the likelihoods. Hence,
     * likelihood is simply a mapping from (word, class) pairs to their marginal
     * likelihood \f$p(w|c)\f$..
     */
    using likelihood_t =
        std::unordered_map<Word, std::unordered_map<Class, double>>;

  public:
    /**
     * @brief Default constructor with empty prior and likelihood.
     */
    NaiveBayesClassifier() = default;

    /**
     * @brief Constructor that initialized this object with the given prior
     * and likelihood.
     *
     * Given prior and likelihood are not checked for validity. It is up to the
     * user of this class to provide valid prior and likelihood using this
     * constructor.
     *
     * @param prior Prior class probability.
     * @param likelihood Marginal likelihood \f$p(w|c)\f$ where \f$w\f$ is a
     * word and \f$c\f$ is a class.
     */
    NaiveBayesClassifier(const prior_t& prior, const likelihood_t& likelihood);

    /**
     * @brief Fit this NaiveBayesClassifier with the given training data and
     * labels.
     *
     * @param x_train vector of document samples NaiveBayesClassifier::sample.
     * @param y_train vector of classes.
     *
     * @return Reference to the fitted version of this object.
     */
    NaiveBayesClassifier& fit(const std::vector<sample>& x_train,
                              const std::vector<Class>& y_train);

    /**
     * @brief Predict the class of a single sample using the already learned
     * parameters.
     *
     * @param x_pred Sample to predict.
     *
     * @return Class of the given sample.
     */
    Class predict(const sample& x_pred) const;

    /**
     * @brief Predict the classes of all samples in the given sample vector.
     *
     * @param x_pred vector of samples to predict.
     *
     * @return Class of each sample in the given order.
     */
    std::vector<Class> predict(const std::vector<sample>& x_pred) const;

    /**
     * @brief Get the prior class distribution.
     *
     * @return const-reference to prior class distribution.
     */
    const prior_t& prior() const;

    /**
     * @brief Get the marginal likelihood distribution.
     *
     * @return const-reference to marginal likelihood distribution.
     */
    const likelihood_t& likelihood() const;

  private:
    std::set<Word> m_dict;
    std::vector<Class> m_class_vec;
    prior_t m_prior;
    likelihood_t m_likelihood;
};

/**
 * @brief Output a string representation of the given NaiveBayesClassifier
 * object.
 *
 * @param os Output stream.
 * @param clf NaiveBayesClassifier object whose string representation will be
 * output.
 *
 * @return Modified output stream.
 */
template <typename Word, typename Class>
std::ostream& operator<<(std::ostream& os,
                         const NaiveBayesClassifier<Word, Class>& clf);

/**
 * @brief Construct a new NaiveBayesClassifier object from the string
 * representation in the given input stream and assign it to the given
 * NaiveBayesClassifier reference.
 *
 * @param is Input stream to read the string representation of a
 * NaiveBayesClassifier.
 * @param clf NaiveBayesClassifier reference to assign the newly constructed
 * NaiveBayesClassifier.
 *
 * @return Modified input stream.
 */
template <typename Word, typename Class>
std::istream& operator>>(std::istream& is,
                         NaiveBayesClassifier<Word, Class>& clf);

/************************** IMPLEMENTATION ********************************/

template <typename Word, typename Class>
NaiveBayesClassifier<Word, Class>::NaiveBayesClassifier(
    const prior_t& prior, const likelihood_t& likelihood)
    : m_dict(), m_class_vec(), m_prior(prior), m_likelihood(likelihood) {

    // store list of classes
    for (const auto& pair : prior) {
        m_class_vec.push_back(pair.first);
    }

    // store the dictionary
    for (const auto& pair : likelihood) {
        m_dict.insert(pair.first);
    }
}

template <typename Word, typename Class>
NaiveBayesClassifier<Word, Class>&
NaiveBayesClassifier<Word, Class>::fit(const std::vector<sample>& x_train,
                                       const std::vector<Class>& y_train) {
    assert(x_train.size() == y_train.size());
    const size_t n_samples = y_train.size();

    m_prior.clear();
    m_likelihood.clear();

    // Compute class prior probabilities
    for (const Class& c : y_train) {
        ++m_prior[c];
    }
    for (auto& pair : m_prior) {
        double& count = pair.second;
        count /= n_samples;
    }

    // Construct class mega documents (concatenate all docs belonging to same
    // class)
    std::unordered_map<Class, sample> class_megadocs;
    for (size_t i = 0; i < x_train.size(); ++i) {
        const sample& smp = x_train[i];
        const Class& cls = y_train[i];

        for (const auto& pair : smp) {
            const Word& word = pair.first;
            const size_t count = pair.second;

            class_megadocs[cls][word] += count;
        }
    }

    // Compute marginal likelihood for each <word,class> pair with Laplace
    // smoothing.
    for (const auto& pair : class_megadocs) {
        const Class& cls = pair.first;
        const sample& smp = pair.second;

        int total_word_count =
            std::accumulate(smp.begin(), smp.end(), 0,
                            [](size_t curr_sum, const auto& sample_pair) {
                                return curr_sum + sample_pair.second;
                            });
        for (const auto& sample_pair : smp) {
            const Word& word = sample_pair.first;
            const size_t count = sample_pair.second;

            m_likelihood[word][cls] = (static_cast<double>(count) + 1) /
                                      (total_word_count + m_dict.size());
        }
    }

    return *this;
}

template <typename Word, typename Class>
Class NaiveBayesClassifier<Word, Class>::predict(const sample& x_pred) const {
    // Log posterior score of each class
    std::unordered_map<Class, double> posterior;

    // initialize MAP score with log class priors
    for (const auto& pair : m_prior) {
        const Class& cls = pair.first;
        const double prob = pair.second;
        posterior[cls] = std::log(prob);
    }

    // Add log marginal likelihood count many times to corresponding class
    // posterior where count is the number of times a word occurs in the given
    // sample x_pred.
    for (const Class& cls : m_class_vec) {
        for (const auto& sample_pair : x_pred) {
            const Word& word = sample_pair.first;
            const size_t count = sample_pair.second;

            bool exists =
                m_likelihood.find(word) != m_likelihood.end() &&
                m_likelihood.at(word).find(cls) != m_likelihood.at(word).end();
            double logprob = std::log(exists ? m_likelihood.at(word).at(cls)
                                             : 1.0 / m_dict.size());
            posterior[cls] += count * logprob;
        }
    }

    // find the class with max posterior
    Class map_cls = std::max_element(posterior.begin(), posterior.end(),
                                     [](const auto& left, const auto& right) {
                                         return left.second < right.second;
                                     })
                        ->first;
    return map_cls;
}

template <typename Word, typename Class>
std::vector<Class> NaiveBayesClassifier<Word, Class>::predict(
    const std::vector<sample>& x_pred) const {
    // predict class of all samples one-by-one
    std::vector<Class> y_pred(x_pred.size());
    std::transform(x_pred.begin(), x_pred.end(), y_pred.begin(),
                   [this](const sample& smp) { return this->predict(smp); });

    return y_pred;
}

template <typename Word, typename Class>
const typename NaiveBayesClassifier<Word, Class>::prior_t&
NaiveBayesClassifier<Word, Class>::prior() const {
    return this->m_prior;
}

template <typename Word, typename Class>
const typename NaiveBayesClassifier<Word, Class>::likelihood_t&
NaiveBayesClassifier<Word, Class>::likelihood() const {
    return this->m_likelihood;
}

template <typename Word, typename Class>
std::ostream& operator<<(std::ostream& os,
                         const NaiveBayesClassifier<Word, Class>& clf) {
    // output floating point numbers with maximum available precision
    os.precision(std::numeric_limits<double>::max_digits10);

    // output class prior probabilities on separate lines
    for (const auto& class_pair : clf.prior()) {
        const auto class_name = class_pair.first;
        const auto prob = class_pair.second;
        os << class_name << ' ' << std::fixed << prob << '\n';
    }

    os << '\n';

    // output marginal likelihood of each <word,class> pair on separate line
    for (const auto& word_pair : clf.likelihood()) {
        const auto& word = word_pair.first;
        const auto& class_cond_prob = word_pair.second;

        for (const auto& class_pair : class_cond_prob) {
            const auto class_name = class_pair.first;
            const auto prob = class_pair.second;
            os << word << ' ' << class_name << ' ' << std::fixed << prob
               << '\n';
        }
    }

    os << std::flush;
    return os;
}

template <typename Word, typename Class>
std::istream& operator>>(std::istream& is,
                         NaiveBayesClassifier<Word, Class>& clf) {
    // accumulators
    typename NaiveBayesClassifier<Word, Class>::prior_t prior;
    typename NaiveBayesClassifier<Word, Class>::likelihood_t likelihood;

    std::string line;
    std::stringstream ss;
    Class class_name;
    double prob = 0;

    // read class prior probabilities
    while (std::getline(is, line)) {
        if (line.empty()) {
            break;
        }
        ss.str(line);
        ss.clear();
        ss >> class_name >> prob;

        prior[class_name] = prob;
    }

    Word word;
    // read marginal likelihood of each <word,class> pair
    while (std::getline(is, line)) {
        ss.str(line);
        ss.clear();
        ss >> word >> class_name >> prob;

        likelihood[word][class_name] = prob;
    }

    // construct a new NaiveBayesClassifier from the read model and assign to
    // given reference
    clf = NaiveBayesClassifier<Word, Class>(prior, likelihood);

    return is;
}

} // namespace ir
