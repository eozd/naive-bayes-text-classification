#pragma once

#include <type_traits>
#include <vector>

namespace ir {

/**
 *
 */
enum AvgType {
    /**
     *
     */
    Micro,
    /**
     *
     */
    Macro,
    /**
     *
     */
    NoAvg
};

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
std::unordered_map<Class, double> precision(const std::vector<Class>& y_true,
                                            const std::vector<Class>& y_pred) {
    std::unordered_map<Class, double> precision;
    std::unordered_map<Class, size_t> positive_count;

    // calculate counts
    for (size_t i = 0; i < y_true.size(); ++i) {
        // update confusion matrix rows
        positive_count[y_pred[i]]++;
        // update confusion matrix diagonal
        if (y_true[i] == y_pred[i]) {
            precision[y_pred[i]]++;
        }
    }

    // calculate fractions
    for (auto& pair : precision) {
        const auto& cls = pair.first;
        auto& value = pair.second;

        value /= positive_count.at(cls);
    }

    return precision;
};

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type,
          std::enable_if_t<(type == AvgType::Micro) ||
                           (type == AvgType::Macro)>* = nullptr,
          typename Class>
double precision(const std::vector<Class>& y_true,
                 const std::vector<Class>& y_pred) {
    if (type == AvgType::Micro) {
        double tp = 0;
        for (size_t i = 0; i < y_true.size(); ++i) {
            tp += (y_true[i] == y_pred[i]);
        }

        return tp / y_true.size();
    } else if (type == AvgType::Macro) {
        auto separate_precision = precision<NoAvg>(y_true, y_pred);

        // return unweighted average
        double total = 0;
        std::for_each(separate_precision.begin(), separate_precision.end(),
                      [&total](const auto& pair) { total += pair.second; });
        return total / separate_precision.size();
    }
};

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
std::unordered_map<Class, double> recall(const std::vector<Class>& y_true,
                                         const std::vector<Class>& y_pred) {
    std::unordered_map<Class, double> recall;
    std::unordered_map<Class, size_t> true_count;

    // calculate counts
    for (size_t i = 0; i < y_true.size(); ++i) {
        // update confusion matrix rows
        true_count[y_true[i]]++;
        // update confusion matrix diagonal
        if (y_true[i] == y_pred[i]) {
            recall[y_pred[i]]++;
        }
    }

    // calculate fractions
    for (auto& pair : recall) {
        const auto& cls = pair.first;
        auto& value = pair.second;

        value /= true_count.at(cls);
    }

    return recall;
};

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type,
          std::enable_if_t<(type == AvgType::Micro) ||
                           (type == AvgType::Macro)>* = nullptr,
          typename Class>
double recall(const std::vector<Class>& y_true,
              const std::vector<Class>& y_pred) {
    if (type == AvgType::Micro) {
        double tp = 0;
        for (size_t i = 0; i < y_true.size(); ++i) {
            tp += (y_true[i] == y_pred[i]);
        }

        return tp / y_true.size();
    } else if (type == AvgType::Macro) {
        auto separate_recall = recall<NoAvg>(y_true, y_pred);

        // return unweighted average
        double total = 0;
        std::for_each(separate_recall.begin(), separate_recall.end(),
                      [&total](const auto& pair) { total += pair.second; });
        return total / separate_recall.size();
    }
};

double f_beta(double precision, double recall, double beta = 1) {
    double beta_sq = beta * beta;
    return (1 + beta_sq) * (precision * recall) /
           ((beta_sq * precision) + recall);
}

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
std::unordered_map<Class, double> f_score(const std::vector<Class>& y_true,
                                          const std::vector<Class>& y_pred,
                                          double beta = 1) {
    const auto separate_precision = precision<NoAvg>(y_true, y_pred);
    const auto separate_recall = precision<NoAvg>(y_true, y_pred);

    std::unordered_map<Class, double> fscores;
    for (const auto& pair : separate_precision) {
        const auto& cls = pair.first;
        const double precision = pair.second;
        const double recall = separate_recall.at(cls);

        fscores[cls] = f_beta(precision, recall, beta);
    }

    return fscores;
};

/**
 *
 * @tparam type
 * @tparam Class
 * @param y_true
 * @param y_pred
 * @return
 */
template <AvgType type,
          std::enable_if_t<(type == AvgType::Micro) ||
                           (type == AvgType::Macro)>* = nullptr,
          typename Class>
double f_score(const std::vector<Class>& y_true,
               const std::vector<Class>& y_pred, double beta = 1) {
    if (type == AvgType::Micro) {
        double micro_precision = precision<Micro>(y_true, y_pred);
        double micro_recall = recall<Micro>(y_true, y_pred);

        return f_beta(micro_precision, micro_recall, beta);
    } else if (type == AvgType::Macro) {
        auto separate_f_score = f_score<NoAvg>(y_true, y_pred);

        // return unweighted average
        double total = 0;
        std::for_each(separate_f_score.begin(), separate_f_score.end(),
                      [&total](const auto& pair) { total += pair.second; });
        return total / separate_f_score.size();
    }
};

} // namespace ir
