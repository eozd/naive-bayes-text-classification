#include "file_manager.hpp"
#include "naive_bayes_classifier.hpp"
#include <fstream>
#include <iostream>

static const std::string FitArg = "--fit";
static const std::string PredictArg = "--predict";
static const std::string NumFeaturesArg = "--num-features";

std::ostream& print_space(std::ostream& os, size_t count) {
    os << std::string(count, ' ');
    return os;
}

void print_usage(char* program_name) {
    std::string header("usage: ");
    std::string param_fit(FitArg + " train_set model_path");
    std::string param_predict(PredictArg + " test_set model_path");
    std::string param_num_features(NumFeaturesArg + " N");

    size_t max_param_len = std::max(param_fit.size(), param_predict.size());

    header += std::string(program_name) + ' ';
    std::cerr << header << '[' << param_fit << ']' << '\n';

    print_space(std::cerr, header.size());
    std::cerr << '[' << param_predict << " [" << param_num_features << ']'
              << ']' << '\n';

    std::cerr << '\n';
    std::cerr
        << "Fit a classifier using a training data, or predict the classes\n"
           "of a test data using an already predict model"
        << '\n';
    std::cerr << '\n';
    std::cerr << "optional arguments:" << '\n';

    std::cerr << "  " << param_fit << '\t' << " Fit a Naive Bayes classifier\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "from given train_set and save the model to model_path"
              << '\n';

    std::cerr << '\n';

    std::cerr << "  " << param_predict << '\t'
              << " Predict the classes of samples\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "at test_set using an already fitted model at model_path"
              << '\n';

    std::cerr << '\n';

    std::cerr << "  " << param_num_features << "\t\t"
              << " Number of features to use during prediction.\n";
    print_space(std::cerr, max_param_len + 4);
    std::cerr << "If not given, all the words are used as features\n";

    std::cerr << std::flush;
}

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
    correct_option = num_features_option == NumFeaturesArg;
    bool only_digits =
        num_features.find_first_not_of("0123456789") == std::string::npos;

    return correct_option && only_digits;
}

void fit(char** argv) {
    std::string train_path(argv[2]);
    std::string model_path(argv[3]);

    ir::doc_term_index doc_terms;
    ir::doc_class_index doc_classes;
    {
        std::ifstream train_file(train_path);
        std::tie(doc_terms, doc_classes) = ir::read_dataset(train_file);
    }

    std::vector<ir::doc_sample> x_train;
    std::vector<ir::DocClass> y_train;
    for (const auto& pair : doc_terms) {
        const size_t id = pair.first;
        const ir::doc_sample& doc = pair.second;
        const ir::DocClass& doc_class = doc_classes[id];

        x_train.push_back(doc);
        y_train.push_back(doc_class);
    }

    ir::NaiveBayesClassifier<std::string, ir::DocClass> clf;
    clf.fit(x_train, y_train);
    {
        std::ofstream model_file(model_path);
        model_file << clf;
    }
}

void predict(char** argv, size_t num_features = 0) {
    std::string test_path(argv[2]);
    std::string model_path(argv[3]);

    ir::NaiveBayesClassifier<std::string, ir::DocClass> clf;
    {
        std::ifstream model_file(model_path);
        model_file >> clf;
    }

    ir::doc_term_index doc_terms;
    ir::doc_class_index doc_classes;
    {
        std::ifstream test_file(test_path);
        std::tie(doc_terms, doc_classes) = ir::read_dataset(test_file);
    }

    std::vector<ir::doc_sample> x_test;
    std::vector<ir::DocClass> y_test;
    for (const auto& pair : doc_terms) {
        const size_t id = pair.first;
        const ir::doc_sample& doc = pair.second;
        const ir::DocClass& doc_class = doc_classes[id];

        x_test.push_back(doc);
        y_test.push_back(doc_class);
    }

    if (num_features != 0) {
        // todo: use mutual information to select features for docs
    }

    const auto y_pred = clf.predict(x_test);

    for (size_t i = 0; i < y_pred.size(); ++i) {
        std::cout << "Test:\t" << y_test[i] << ",\tPred:\t" << y_pred[i]
                  << std::endl;
    }
}

int main(int argc, char** argv) {
    if (!correct_args(argc, argv)) {
        print_usage(argv[0] + 2);
        return -1;
    }

    std::string option(argv[1]);
    if (option == FitArg) {
        fit(argv);
    } else if (option == PredictArg) {
        if (argc == 6) {
            size_t num_features = std::stoul(argv[5]);
            predict(argv, num_features);
        } else {
            predict(argv);
        }
    }

    return 0;
}
