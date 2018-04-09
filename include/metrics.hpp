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

#include <type_traits>
#include <vector>

namespace ir {

/**
 * @brief Enum denoting the average type to use.
 */
enum AvgType {
    /**
     * @brief Perform micro averaging.
     */
    Micro,
    /**
     * @brief Perform macro averaging.
     */
    Macro,
    /**
     * @brief Perform no averageng.
     */
    NoAvg
};

/**
 * @brief Compute and return precision of each class separately without
 * averaging
 *
 * Precision for class \f$i\f$ is calculated as
 *
 * \f[
 *     \frac{TP_i}{P_i}
 * \f]
 *
 * where \f$TP_i\f$ is the number of correct classifications of class \f$i\f$
 * and \f$P_i\f$ is the number of times class \f$i\f$ is predicted.
 *
 * @tparam type Averaging type. This function is used if type is AvgType::NoAvg.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 *
 * @return Mapping from each class to its precision value.
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
ir::unordered_enum_map <Class, double> precision(const std::vector<Class>& y_true,
                                            const std::vector<Class>& y_pred) {
    ir::unordered_enum_map <Class, double> precision;
    ir::unordered_enum_map <Class, size_t> positive_count;

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
 * @brief Compute and return averaged precision over all classes.
 *
 * Averaging to perform is given as a template parameter.
 *
 * Micro averaged precision is calculated as
 *
 * \f[
 *     \frac{\sum_i TP_i}{N}
 * \f]
 *
 * where \f$TP_i\f$ is the number of correct classifications for class \f$i\f$
 * and \f$N\f$ is the number of samples.
 *
 * Macro averaged precision is calculated by finding the precision of each
 * class separately and then returning the unweighted average of those values.
 *
 * @tparam type Type of averaging to perform. This function is chosen if type
 * is AvgType::Micro or AvgType::Macro.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 *
 * @return Micro or macro averaged precision.
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
 * @brief Compute and return recall of each class separately without averaging
 *
 * Recall for class \f$i\f$ is calculated as
 *
 * \f[
 *     \frac{TP_i}{T_i}
 * \f]
 *
 * where \f$TP_i\f$ is the number of correct classifications of class \f$i\f$
 * and \f$T_i\f$ is the number of times class is actually \f$i\f$.
 *
 * @tparam type Averaging type. This function is used if type is AvgType::NoAvg.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 *
 * @return Mapping from each class to its recall value.
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
ir::unordered_enum_map <Class, double> recall(const std::vector<Class>& y_true,
                                         const std::vector<Class>& y_pred) {
    ir::unordered_enum_map <Class, double> recall;
    ir::unordered_enum_map <Class, size_t> true_count;

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
 * @brief Compute and return averaged recall over all classes.
 *
 * Averaging to perform is given as a template parameter.
 *
 * Micro averaged recall is calculated as
 *
 * \f[
 *     \frac{\sum_i TP_i}{N}
 * \f]
 *
 * where \f$TP_i\f$ is the number of correct classifications for class \f$i\f$
 * and \f$N\f$ is the number of samples.
 *
 * Macro averaged recall is calculated by finding the recall of each
 * class separately and then returning the unweighted average of those values.
 *
 * @tparam type Type of averaging to perform. This function is chosen if type
 * is AvgType::Micro or AvgType::Macro.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 *
 * @return Micro or macro averaged recall.
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

/**
 * @brief Compute F-beta score.
 *
 * F-beta score is calculated as follows:
 *
 * \f[
 *     (1 + \beta^2)\frac{PR}{(\beta^2P) + R}
 * \f]
 *
 * where \f$\beta\f$ is function parameter, \f$P\f$ is precision and \f$R\f$
 * is recall.
 *
 * @param precision Precision value.
 * @param recall Recall value.
 * @param beta Beta parameter.
 *
 * @return F-beta score.
 */
double f_beta(double precision, double recall, double beta = 1) {
    double beta_sq = beta * beta;
    return (1 + beta_sq) * (precision * recall) /
           ((beta_sq * precision) + recall);
}

/**
 * @brief Compute and return f-score of each class separately without averaging
 *
 * F-score for class \f$i\f$ is calculated as
 *
 * \f[
 *     (1 + \beta^2)\frac{P_iR_i}{(\beta^2P_i) + R_i}
 * \f]
 *
 * where \f$\beta\f$ is F score parameter, \f$P_i\f$ is precision of class
 * \f$i\f$ and \f$R_i\f$ is recall of class \f$i\f$.
 *
 * @tparam type Averaging type. This function is used if type is AvgType::NoAvg.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 * @param beta Beta parameter of f-score.
 *
 * @return Mapping from each class to its f-score value.
 */
template <AvgType type, std::enable_if_t<type == AvgType::NoAvg>* = nullptr,
          typename Class>
ir::unordered_enum_map <Class, double> f_score(const std::vector<Class>& y_true,
                                          const std::vector<Class>& y_pred,
                                          double beta = 1) {
    const auto separate_precision = precision<NoAvg>(y_true, y_pred);
    const auto separate_recall = precision<NoAvg>(y_true, y_pred);

    ir::unordered_enum_map <Class, double> fscores;
    for (const auto& pair : separate_precision) {
        const auto& cls = pair.first;
        const double precision = pair.second;
        const double recall = separate_recall.at(cls);

        fscores[cls] = f_beta(precision, recall, beta);
    }

    return fscores;
};

/**
 * @brief Compute and return averaged f-score over all classes.
 *
 * Averaging to perform is given as a template parameter.
 *
 * Micro averaged F score is calculated as
 *
 * \f[
 *     (1 + \beta^2)\frac{\hat{P}\hat{R}}{(\beta^2\hat{P}) + \hat{R}}
 * \f]
 *
 * where \f$\hat{P}\f$ is micro averaged precision and \f$\hat{R}\f$ is micro
 * averaged recall.
 *
 * Macro averaged f-score is calculated by finding the f-score of each
 * class separately and then returning the unweighted average of those values.
 *
 * @tparam type Type of averaging to perform. This function is chosen if type
 * is AvgType::Micro or AvgType::Macro.
 * @tparam Class Type of classes to classify the documents to. This can be any
 * type of object satisfying equality constraint (integer, std::string, custom
 * enum, etc.)
 *
 * @param y_true True labels.
 * @param y_pred Predicted labels.
 * @param beta Beta parameter of f-score.
 *
 * @return Micro or macro averaged f-score.
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
