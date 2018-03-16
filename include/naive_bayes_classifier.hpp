#pragma once

#include <iostream>
#include <vector>
#include <sstream>

#include "defs.hpp"

namespace ir {

template <typename Word, typename Class> class NaiveBayesClassifier {
  public:
    using prior_t = std::unordered_map<Class, double>;
    using likelihood_t =
        std::unordered_map<Word, std::unordered_map<Class, double>>;

  public:
    NaiveBayesClassifier(const prior_t& prior, const likelihood_t& likelihood)
        : m_prior(prior), m_likelihood(likelihood) {}

    NaiveBayesClassifier& fit(const std::vector<doc_sample>& x_train,
                              const std::vector<Class> y_train) {
        return *this;
    }

    Class predict(const doc_sample& x_pred) { return {}; }

    std::vector<Class> predict(const std::vector<doc_sample>& x_pred) {
        return {};
    }

    const prior_t& prior() const { return this->m_prior; }
    const likelihood_t& likelihood() const { return this->m_likelihood; }

  private:
    prior_t m_prior;
    likelihood_t m_likelihood;
};

template <typename Word, typename Class>
std::ostream& operator<<(std::ostream& os,
                         const NaiveBayesClassifier<Word, Class>& clf) {
    for (const auto& class_pair : clf.prior()) {
        const auto class_name = class_pair.first;
        const auto prob = class_pair.second;
        os << class_name << ' ' << prob << '\n';
    }

    for (const auto& word_pair : clf.likelihood()) {
        const auto& word = word_pair.first;
        const auto& class_cond_prob = word_pair.second;

        for (const auto& class_pair : class_cond_prob) {
            const auto class_name = class_pair.first;
            const auto prob = class_pair.second;
            os << word << ' ' << class_name << ' ' << prob << '\n';
        }
    }

    os << std::flush;
    return os;
}

template <typename Word, typename Class>
std::istream& operator>>(std::istream& is,
                         NaiveBayesClassifier<Word, Class>& clf) {
    NaiveBayesClassifier<Word, Class>::prior_t prior;
    NaiveBayesClassifier<Word, Class>::likelihood_t likelihood;

    std::string line;
    std::stringstream ss;
    Class class_name;
    double prob;
    while (std::getline(is, line)) {
        if (line.empty()) {
            break;
        }
        ss.str(line);
        ss >> class_name >> prob;

        prior[class_name] = prob;
    }

    Word word;
    while (std::getline(is, line)) {
        ss.str(line);
        ss >> word >> class_name >> prob;

        likelihood[word][class_name] = prob;
    }

    clf = NaiveBayesClassifier<Word, Class>(prior, likelihood);

    return is;
}
} // namespace ir
