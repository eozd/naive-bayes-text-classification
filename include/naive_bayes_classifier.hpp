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

template <typename Word, typename Class> class NaiveBayesClassifier {
  public:
    using sample = std::unordered_map<Word, size_t>;
    using prior_t = std::unordered_map<Class, double>;
    using likelihood_t =
        std::unordered_map<Word, std::unordered_map<Class, double>>;

  public:
    NaiveBayesClassifier() = default;
    NaiveBayesClassifier(const prior_t& prior, const likelihood_t& likelihood)
        : m_dict(), m_class_vec(), m_prior(prior), m_likelihood(likelihood) {

        for (const auto& pair : prior) {
            m_class_vec.push_back(pair.first);
        }

        for (const auto& pair : likelihood) {
            const Word& word = pair.first;
            m_dict.insert(word);
        }
    }

    NaiveBayesClassifier& fit(const std::vector<sample>& x_train,
                              const std::vector<Class>& y_train) {
        assert(x_train.size() == y_train.size());
        const size_t n_samples = y_train.size();

        m_prior.clear();
        m_likelihood.clear();

        for (const Class& c : y_train) {
            ++m_prior[c];
        }
        for (auto& pair : m_prior) {
            double& count = pair.second;
            count /= n_samples;
        }

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

    Class predict(const sample& x_pred) const {
        std::unordered_map<Class, double> map_score;
        for (const auto& pair : m_prior) {
            const Class& cls = pair.first;
            const double prob = pair.second;
            map_score[cls] = std::log(prob);
        }

        for (const Class& cls : m_class_vec) {
            for (const auto& sample_pair : x_pred) {
                const Word& word = sample_pair.first;
                const size_t count = sample_pair.second;

                bool exists = m_likelihood.find(word) != m_likelihood.end() &&
                              m_likelihood.at(word).find(cls) !=
                                  m_likelihood.at(word).end();
                double logprob = std::log(exists ? m_likelihood.at(word).at(cls)
                                                 : 1.0 / m_dict.size());
                map_score[cls] += count * logprob;
            }
        }

        Class map_cls =
            std::max_element(map_score.begin(), map_score.end(),
                             [](const auto& left, const auto& right) {
                                 return left.second < right.second;
                             })
                ->first;
        return map_cls;
    }

    std::vector<Class> predict(const std::vector<sample>& x_pred) {
        std::vector<Class> y_pred(x_pred.size());
        std::transform(
            x_pred.begin(), x_pred.end(), y_pred.begin(),
            [this](const sample& smp) { return this->predict(smp); });

        return y_pred;
    }

    const prior_t& prior() const { return this->m_prior; }
    const likelihood_t& likelihood() const { return this->m_likelihood; }

  private:
    std::set<Word> m_dict;
    std::vector<Class> m_class_vec;
    prior_t m_prior;
    likelihood_t m_likelihood;
};

template <typename Word, typename Class>
std::ostream& operator<<(std::ostream& os,
                         const NaiveBayesClassifier<Word, Class>& clf) {
    os.precision(std::numeric_limits<double>::max_digits10);

    for (const auto& class_pair : clf.prior()) {
        const auto class_name = class_pair.first;
        const auto prob = class_pair.second;
        os << class_name << ' ' << std::fixed << prob << '\n';
    }

    os << '\n';

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
    typename NaiveBayesClassifier<Word, Class>::prior_t prior;
    typename NaiveBayesClassifier<Word, Class>::likelihood_t likelihood;

    std::string line;
    std::stringstream ss;
    Class class_name;
    double prob;
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
    while (std::getline(is, line)) {
        ss.str(line);
        ss.clear();
        ss >> word >> class_name >> prob;

        likelihood[word][class_name] = prob;
    }

    clf = NaiveBayesClassifier<Word, Class>(prior, likelihood);

    return is;
}
} // namespace ir
