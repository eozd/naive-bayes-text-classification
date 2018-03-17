#include <iostream>
#include <fstream>
#include "naive_bayes_classifier.hpp"
#include "file_manager.hpp"

static const std::string FitArg = "--fit";
static const std::string PredictArg = "--predict";

std::ostream& print_space(std::ostream& os, size_t count) {
    os << std::string(count, ' ');
    return os;
}

void print_usage(char* program_name) {
    std::string header("usage: ");
    std::string param_fit(FitArg + " train_set model_path");
    std::string param_predict(PredictArg + " test_set model_path");

    size_t max_param_len = std::max(param_fit.size(), param_predict.size());

    header += std::string(program_name) + ' ';
    std::cerr << header << '[' << param_fit << ']' << '\n';

    print_space(std::cerr, header.size());

    std::cerr << '[' << param_predict << ']' << '\n';
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
    std::cerr << "  " << param_predict << '\t'
              << " Predict the classes of samples\n";

    print_space(std::cerr, max_param_len + 4);

    std::cerr << "at test_set using an already fitted model at model_path"
              << '\n';
    std::cerr << std::flush;
}

bool correct_args(int argc, char** argv) {
    if (argc != 4) {
        return false;
    }
    std::string option(argv[1]);
    return option == FitArg || option == PredictArg;
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

void predict(char** argv) {
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

    const auto y_pred = clf.predict(x_test);

    for (size_t i = 0; i < y_pred.size(); ++i) {
        std::cout << "Test:\t" << y_test[i] << ",\tPred:\t" << y_pred[i] << std::endl;
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
        predict(argv);
    }

    return 0;
}
