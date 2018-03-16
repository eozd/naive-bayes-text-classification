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
ir::raw_doc_index docs_from_files(const std::vector<std::string>& file_list) {
    ir::raw_doc_index all_docs;
    for (const auto& filepath : file_list) {
        std::ifstream ifs(filepath);
        // get all the docs in the current file
        auto docs_per_file = ir::parse_file(ifs);
        // store all the docs in the result variable
        all_docs.insert(docs_per_file.begin(), docs_per_file.end());
    }

    return all_docs;
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
    auto raw_docs = docs_from_files(ir::get_data_file_list());

    // handle special html character sequences
    for (auto& pair : raw_docs) {
        auto& doc = pair.second;
        ir::convert_html_special_chars(doc);
    }

    // tokenize and normalize the documents
    auto term_docs = terms_from_raw_docs(tokenizer, raw_docs);

    std::cerr << "OK!" << std::endl;
    std::cerr << "Writing train and test dataset files..." << std::flush;

    std::cerr << "OK!" << std::endl;

    return 0;
}