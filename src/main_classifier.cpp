#include "feature_selection.hpp"
#include "file_manager.hpp"
#include "metrics.hpp"
#include "naive_bayes_classifier.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

/**
 * @brief Fit argument string.
 */
static const std::string FitArg = "--fit";
/**
 * @brief Predict argument string.
 */
static const std::string PredictArg = "--predict";
/**
 * @brief Number of features argument string.
 */
static const std::string NumFeaturesArg = "--num-features";

/**
 * @brief Output count many space characters to the given output stream.
 *
 * @param os Output stream.
 * @param count Number of spaces to output.
 *
 * @return Modified output stream.
 */
std::ostream& print_space(std::ostream& os, size_t count) {
    os << std::string(count, ' ');
    return os;
}

/**
 * @brief Print program usage string.
 *
 * @param program_name Name of the program.
 */
void print_usage(char* program_name) {
    std::string header("usage: ");
    std::string param_fit(FitArg + " train_set model_path");
    std::string param_predict(PredictArg + " test_set model_path");
    std::string param_num_features(NumFeaturesArg + " N");

    size_t max_param_len = std::max(param_fit.size(), param_predict.size());

    header += std::string(program_name) + ' ';
    std::cerr << header << '[' << param_fit << " [" << param_num_features << ']'
              << ']' << '\n';

    print_space(std::cerr, header.size());
    std::cerr << '[' << param_predict << ']' << '\n';

    std::cerr << '\n';
    std::cerr
        << "Fit a classifier using a training set; or predict the classes\n"
           "of a test set using an already fitted model."
        << '\n';
    std::cerr << '\n';
    std::cerr << "optional arguments:" << '\n';

    std::cerr << "  " << param_fit << '\t'
              << " Fit a Naive Bayes classifier from given\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "train_set and save the model to model_path." << '\n';

    std::cerr << '\n';

    std::cerr << "  " << param_num_features << "\t\t"
              << " Number of features to use during training.\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "Best N features are chosen using Mutual Information.\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "If not given, all the words are used as features.\n";

    std::cerr << '\n';

    std::cerr << "  " << param_predict << '\t'
              << " Predict the classes of samples in test_set\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "using an already fitted model in model_path\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "and output the results to STDOUT." << '\n';

    std::cerr << std::flush;
}

/**
 * @brief Check if the program arguments are given correctly.
 *
 * @param argc Number of arguments as given in int main(int argc, char** argv).
 * @param argv Argument string array as given in int main(int argc, char**
 * argv).
 *
 * @return true if the given arguments are correct; false, otherwise.
 */
bool correct_args(int argc, char** argv) {
    if (!(argc == 4 || argc == 6)) {
        return false;
    }
    std::string option(argv[1]);
    bool correct_option = option == FitArg || option == PredictArg;
    if (argc == 4) {
        return correct_option;
    }

    std::string num_features_option(argv[4]);
    std::string num_features(argv[5]);
    correct_option = option == FitArg && num_features_option == NumFeaturesArg;
    bool only_digits =
        num_features.find_first_not_of("0123456789") == std::string::npos;

    return correct_option && only_digits;
}

/**
 * @brief Fit a Naive Bayes Classifier with the given number of features.
 *
 * This function trains a Naive Bayes Classifier from the given training set
 * and saves the model to the given output path.
 *
 * @param train_path Path to the training set.
 * @param model_path Path to which the model is going to be saved.
 * @param num_features Number of features to use. If not given, all the features
 * are used.
 */
void fit(const std::string& train_path, const std::string& model_path,
         size_t num_features = 0) {
    ir::doc_term_index doc_terms;
    ir::doc_class_index doc_classes;
    {
        std::ifstream train_file(train_path);
        std::tie(doc_terms, doc_classes) = ir::read_dataset(train_file);
    }

    // construct training set feature (x) and label (y) sets, and a set of
    // classes.
    std::vector<ir::doc_sample> x_train;
    std::vector<ir::DocClass> y_train;
    std::set<ir::DocClass> class_dict;
    for (const auto& pair : doc_terms) {
        const size_t id = pair.first;
        const ir::doc_sample& doc = pair.second;
        const ir::DocClass& doc_class = doc_classes[id];

        x_train.push_back(doc);
        y_train.push_back(doc_class);
        class_dict.insert(doc_class);
    }

    // choose important words via mutual information if num_features is given
    if (num_features != 0) {
        // get most important words found by mutual info
        auto top_words_per_class = ir::get_top_words_per_class(
            x_train, y_train, class_dict, num_features);
        
        for (const auto& pair : top_words_per_class) {
            const auto& cls = pair.first;
            const auto& word_vec = pair.second;
            
            std::string cls_str = ir::to_string(cls);
            std::cerr << cls_str << std::endl;
            for (size_t i = 0; i < cls_str.size(); ++i) std::cerr << '-';
            std::cerr << std::endl;
            for (const auto& word : word_vec) {
                std::cerr << word << std::endl;
            }
            std::cerr << std::endl;
        }

        // sort each word vector
        for (auto& pair : top_words_per_class) {
            auto& word_vec = pair.second;
            std::sort(word_vec.begin(), word_vec.end());
        }

        // remove unimportant words
        ir::remove_unimportant_words(x_train, y_train, top_words_per_class);
    }

    // fit naive bayes clf
    ir::NaiveBayesClassifier<std::string, ir::DocClass> clf;
    clf.fit(x_train, y_train);

    // save the classifier
    std::ofstream model_file(model_path);
    model_file << clf;
}

template <typename LeftVal, typename RightVal>
std::ostream& print_aligned(std::ostream& os, const LeftVal& left_val,
                            const RightVal& right_val, size_t width,
                            size_t precision) {
    os << std::setw(width) << std::left << left_val << std::setw(width)
       << std::right << std::fixed << std::setprecision(precision) << right_val
       << std::endl;
    return os;
};

void print_prediction_stats(const std::vector<ir::DocClass>& y_test,
                            const std::vector<ir::DocClass>& y_pred) {
    using namespace std;
    constexpr size_t val_width = 10;
    constexpr size_t precision = 4;
    // output prediction statistics to STDERR
    cerr << "Micro Averaged Stats" << endl;
    cerr << "--------------------" << endl;
    print_aligned(cerr, "Precision:", ir::precision<ir::Micro>(y_test, y_pred),
                  val_width, precision);
    print_aligned(cerr, "Recall:", ir::recall<ir::Micro>(y_test, y_pred),
                  val_width, precision);
    print_aligned(cerr, "F1 score:", ir::f_score<ir::Micro>(y_test, y_pred),
                  val_width, precision);

    cerr << endl;

    cerr << "Macro Averaged Stats" << endl;
    cerr << "--------------------" << endl;
    print_aligned(cerr, "Precision:", ir::precision<ir::Macro>(y_test, y_pred),
                  val_width, precision);
    print_aligned(cerr, "Recall:", ir::recall<ir::Macro>(y_test, y_pred),
                  val_width, precision);
    print_aligned(cerr, "F1 score:", ir::f_score<ir::Macro>(y_test, y_pred),
                  val_width, precision);

    cerr << endl;

    const auto separate_precision = ir::precision<ir::NoAvg>(y_test, y_pred);
    const auto separate_recall = ir::recall<ir::NoAvg>(y_test, y_pred);
    const auto separate_f_score = ir::f_score<ir::NoAvg>(y_test, y_pred);
    cerr << "Unaveraged Stats" << endl;
    cerr << "----------------" << endl;
    cerr << "Precision:\n";
    for (const auto& pair : separate_precision) {
        print_space(cerr, 4);
        print_aligned(cerr, ir::to_string(pair.first) + ":", pair.second,
                      val_width, precision);
    }
    cerr << endl;
    cerr << "Recall:\n";
    for (const auto& pair : separate_recall) {
        print_space(cerr, 4);
        print_aligned(cerr, ir::to_string(pair.first) + ":", pair.second,
                      val_width, precision);
    }
    cerr << endl;
    cerr << "F1-score:\n";
    for (const auto& pair : separate_f_score) {
        print_space(cerr, 4);
        print_aligned(cerr, ir::to_string(pair.first) + ":", pair.second,
                      val_width, precision);
    }
}

/**
 * @brief Predict the classes of all samples in the given test set and output
 * the results to STDOUT.
 *
 * @param test_path Path to the test set.
 * @param model_path Path to an already fitted model file.
 */
void predict(const std::string& test_path, const std::string& model_path) {
    // read the classifier
    ir::NaiveBayesClassifier<std::string, ir::DocClass> clf;
    {
        std::ifstream model_file(model_path);
        model_file >> clf;
    }

    // read test set
    ir::doc_term_index doc_terms;
    ir::doc_class_index doc_classes;
    {
        std::ifstream test_file(test_path);
        std::tie(doc_terms, doc_classes) = ir::read_dataset(test_file);
    }

    // construct test features (x) and labels (y)
    std::vector<size_t> id_vec;
    std::vector<ir::doc_sample> x_test;
    std::vector<ir::DocClass> y_test;
    for (const auto& pair : doc_terms) {
        const size_t id = pair.first;
        const ir::doc_sample& doc = pair.second;
        const ir::DocClass& doc_class = doc_classes[id];

        id_vec.push_back(id);
        x_test.push_back(doc);
        y_test.push_back(doc_class);
    }

    // predict test features
    const auto y_pred = clf.predict(x_test);

    // output test and prediction labels
    for (size_t i = 0; i < id_vec.size(); ++i) {
        std::cout << "ID: " << std::setw(5) << std::right << id_vec[i] << " | "
                  << "Test: " << std::setw(10) << std::right << y_test[i]
                  << " | "
                  << "Pred: " << std::setw(10) << std::right << y_pred[i]
                  << '\n';
    }
    std::cout << std::flush;

    print_prediction_stats(y_test, y_pred);
}

/**
 * @brief Main classifier program.
 *
 * This function checks if the given arguments are correct, then fits a model
 * or predicts the samples in a test set using an already fitted model.
 *
 * @param argc Number of arguments.
 * @param argv Arguments (array of C-strings).
 *
 * @return 0 if no errors occur; -1 if incorrect arguments are given.
 */
int main(int argc, char** argv) {
    if (!correct_args(argc, argv)) {
        print_usage(argv[0] + 2);
        return -1;
    }

    std::string option(argv[1]);
    if (option == FitArg) {
        std::string train_path(argv[2]);
        std::string model_path(argv[3]);

        if (argc == 6) {
            size_t num_features = std::stoul(argv[5]);
            fit(train_path, model_path, num_features);
        } else {
            fit(train_path, model_path);
        }
    } else if (option == PredictArg) {
        std::string test_path(argv[2]);
        std::string model_path(argv[3]);

        predict(test_path, model_path);
    }

    return 0;
}
