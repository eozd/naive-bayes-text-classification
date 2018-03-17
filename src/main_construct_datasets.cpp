#include <algorithm>
#include <fstream>
#include <iostream>
#include <tokenizer.hpp>

#include "doc_preprocessor.hpp"
#include "file_manager.hpp"
#include "parser.hpp"

/**
 * @brief Return an index from document IDs to raw document content constructed
 * from all the documents in the given file_list.
 *
 * @param file_list vector of document paths containing the individual
 * documents to be extracted using ir::parse_file.
 *
 * @return Mapping from document ID to raw document content.
 */
std::tuple<ir::raw_doc_index, ir::doc_class_index, ir::raw_doc_index,
           ir::doc_class_index>
docs_from_files(const std::vector<std::string>& file_list) {
    ir::raw_doc_index train_docs, test_docs;
    ir::doc_class_index train_classes, test_classes;

    ir::raw_doc_index docs;
    ir::doc_type_index doc_types;
    ir::doc_multiclass_index doc_classes;
    for (const auto& filepath : file_list) {
        std::ifstream ifs(filepath);
        // get all the docs in the current file
        std::tie(docs, doc_types, doc_classes) = ir::parse_file(ifs);
        // put each document to its corresponding container (train/test)
        for (const auto& pair : doc_types) {
            const size_t id = pair.first;
            const ir::DocType type = pair.second;
            const auto& doc = docs[id];
            auto classes = doc_classes[id];

            // get rid of unrelated classes
            classes.erase(std::remove(classes.begin(), classes.end(),
                                      ir::DocClass::Other),
                          classes.end());

            // if doc belongs to more than one target class, don't use it.
            if (classes.size() != 1) {
                continue;
            }
            // put the document and its class to corresponding container
            // (train/test)
            switch (type) {
            case ir::DocType::Train:
                train_docs[id] = doc;
                train_classes[id] = classes[0];
                break;
            case ir::DocType::Test:
                test_docs[id] = doc;
                test_classes[id] = classes[0];
                break;
            default:
                break;
            }
        }
    }

    return std::make_tuple(train_docs, train_classes, test_docs, test_classes);
};

/**
 * @brief Return an index from document IDs to vectors of normalized terms in
 * the corresponding documents.
 *
 * @param raw_docs Index from document IDs to raw document content.
 *
 * @return Mapping from document IDs to vectors of normalized terms.
 */
ir::doc_term_index terms_from_raw_docs(ir::Tokenizer& tokenizer,
                                       const ir::raw_doc_index& raw_docs) {
    ir::doc_term_index term_docs;
    for (const auto& pair : raw_docs) {
        const size_t id = pair.first;
        const auto& raw_doc = pair.second;
        // get all the normalized terms in the raw document content and store in
        // document id
        term_docs[id] = tokenizer.get_doc_terms(raw_doc);
    }
    return term_docs;
}

/**
 * @brief Main routine to parse Reuters sgm files, build the positional inverted
 * index and write the dictionary to ir::DICT_PATH and the index to
 * ir::INDEX_PATH.
 *
 * @return 0 if successful.
 */
int main() {
    std::cerr << "Constructing train and test datasets..." << std::flush;
    ir::Tokenizer tokenizer;
    // parse the files and read the docs
    ir::raw_doc_index train_docs, test_docs;
    ir::doc_class_index train_classes, test_classes;
    std::tie(train_docs, train_classes, test_docs, test_classes) =
        docs_from_files(ir::get_data_file_list());

    // handle special html character sequences
    for (auto& pair : train_docs) {
        auto& doc = pair.second;
        ir::convert_html_special_chars(doc);
    }
    for (auto& pair : test_docs) {
        auto& doc = pair.second;
        ir::convert_html_special_chars(doc);
    }

    // tokenize and normalize the documents
    auto train_doc_terms_counts = terms_from_raw_docs(tokenizer, train_docs);
    auto test_doc_terms_counts = terms_from_raw_docs(tokenizer, test_docs);

    std::cerr << "OK!" << std::endl;
    std::cerr << "Writing train and test dataset files..." << std::flush;

    {
        std::ofstream ofs(ir::TRAIN_SET_PATH, std::ios_base::trunc);
        ir::write_dataset(ofs, train_doc_terms_counts, train_classes);
    }
    {
        std::ofstream ofs(ir::TEST_SET_PATH, std::ios_base::trunc);
        ir::write_dataset(ofs, test_doc_terms_counts, test_classes);
    }

    std::cerr << "OK!" << std::endl;
    std::cerr << train_doc_terms_counts.size()
              << " documents was indexed to construct the train dataset at "
              << ir::TRAIN_SET_PATH << std::endl;
    std::cerr << test_doc_terms_counts.size()
              << " documents was indexed to construct the test  dataset at "
              << ir::TEST_SET_PATH << std::endl;

    return 0;
}