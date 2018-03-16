#include <iostream>
#include <fstream>
#include <naive_bayes_classifier.hpp>
#include <defs.hpp>
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

int main(int argc, char** argv) {
    if (!correct_args(argc, argv)) {
        print_usage(argv[0] + 2);
        return -1;
    }

    std::string option(argv[1]);
    std::string dataset_path(argv[2]);
    std::string model_path(argv[3]);

    ir::NaiveBayesClassifier<ir::DocClass> clf;
    if (option == FitArg) {
        std::ifstream train_file(dataset_path);
        // build and fit model
        std::ofstream model_file(model_path);
        model_file << clf;
    } else if (option == PredictArg) {
        {
            std::ifstream model_file(model_path);
            model_file >> clf;
        }
        ir::doc_term_index doc_terms;
        ir::doc_class_index doc_classes;
        {
            std::ifstream test_file(dataset_path);
            std::tie(doc_terms, doc_classes) = ir::read_dataset(test_file);
        }
        // predict classes
        // output classes to STDOUT
    }
    return 0;
}
