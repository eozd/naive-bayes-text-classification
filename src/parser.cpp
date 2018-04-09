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

#include "parser.hpp"

#include <cassert>
#include <iostream>

/**
 * @brief Get the ID of a document with the given header line.
 *
 * Header line of a document starts with ir::DOC_HEADER. ID of a document in the
 * header line is specified with ir::ID_FIELD. This function finds the ID field
 * in the header line and returns it as an integer.
 *
 * @param header_line Header line of a Reuters document starting with
 * ir::DOC_HEADER.
 *
 * @return ID of the document.
 */
int get_doc_id(const std::string& header_line) {
    // header start index
    size_t newid_pos = header_line.find(ir::ID_FIELD);
    // number beginning index
    size_t num_beg_pos = newid_pos + ir::ID_FIELD.size();
    // number end index
    size_t num_end_pos = header_line.find('\"', num_beg_pos);

    std::string num_str =
        header_line.substr(num_beg_pos, num_end_pos - num_beg_pos);

    return std::atoi(num_str.c_str());
}

/**
 * @brief Get the type of a document (train/test) with the given header line.
 *
 * Header line of a document starts with ir::DOC_HEADER. Type of a document in
 * the header line is specified with ir::TRAIN_TEST_FIELD. This function finds
 * the type field in the header line and returns the corresponding ir::DocType
 * value.
 *
 * @param header_line Header line of a Reuters document starting with
 * ir::DOC_HEADER.
 * @return Type of the document (train/test).
 */
ir::DocType get_doc_type(const std::string& header_line) {
    // header start index
    size_t split_pos = header_line.find(ir::TRAIN_TEST_FIELD);
    // number beginning index
    size_t type_beg_pos = split_pos + ir::TRAIN_TEST_FIELD.size();
    // number end index
    size_t type_end_pos = header_line.find('\"', type_beg_pos);

    std::string type =
        header_line.substr(type_beg_pos, type_end_pos - type_beg_pos);

    if (type == ir::TRAIN_KEY) {
        return ir::DocType::Train;
    } else if (type == ir::TEST_KEY) {
        return ir::DocType::Test;
    }
    return ir::DocType::Other;
}

/**
 * @brief Find the next document in the input stream and return its raw content.
 *
 * Raw content of a document is defined as the text inside its ir::TXT_BEG_TAG
 * and ir::TXT_END_TAG s.
 *
 * This function finds the next ir::TXT_BEG_TAG in the input stream and gets all
 * the text until ir::TXT_END_TAG is encountered.
 *
 * @param is Input stream from which next raw document will be extracted.
 *
 * @return Raw content of the next document in the input stream.
 *
 * @throws std::invalid_argument if input stream doesn't contain ir::TXT_BEG_TAG
 * and ir::TXT_END_TAG fields.
 */
ir::raw_doc get_next_doc(std::istream& is) {
    std::string line;
    size_t beg_header_pos;
    size_t end_header_pos;
    while (std::getline(is, line)) {
        beg_header_pos = line.find(ir::TXT_BEG_TAG);
        // if current line contains TXT_BEG_TAG
        if (beg_header_pos != std::string::npos) {
            std::string text;

            // read all the text until TXT_END_TAG is encountered
            do {
                std::getline(is, line);
                end_header_pos = line.find(ir::TXT_END_TAG);
                text += "\n" + line.substr(0, end_header_pos);
            } while (end_header_pos == std::string::npos);

            return text;
        }
    }
    // we should never come here if the input is proper
    throw std::invalid_argument("Input stream does not contain " +
                                ir::TXT_BEG_TAG + " and " + ir::TXT_END_TAG +
                                " fields");
}

/**
 * @brief Return the text between given tags.
 *
 * This function finds and returns the text between given two tags in the
 * given document. This is done by finding the positions of beg_tag and end_tag
 * and returning the substring between indices adjusted to exclude tags
 * themselves.
 *
 * If beg_tag is not in doc_text, then empty string is returned.
 * If end_tag is not in doc_text, then an assertion error is given.
 *
 * @param doc_text Text containing beg_tag and end_tag
 * @param beg_tag Beginning tag.
 * @param end_tag End tag.
 *
 * @return The text between beg_tag and end_tag.
 */
std::string text_between_tags(const std::string& doc_text,
                              const std::string& beg_tag,
                              const std::string& end_tag) {
    size_t tag_beg_pos = doc_text.find(beg_tag);
    // if no beg_tag, return empty string
    if (tag_beg_pos == std::string::npos) {
        return "";
    }

    size_t tag_end_pos = doc_text.find(end_tag);
    assert(tag_end_pos != std::string::npos);

    // don't include the tag itself
    size_t beg_index = tag_beg_pos + beg_tag.size();
    size_t len = tag_end_pos - beg_index;

    return doc_text.substr(beg_index, len);
}

/**
 * @brief Parse the next document topic list by advancing the stream until a
 * topic list is found and extracting the topics, and return the result.
 *
 * A topic list is a line of the following structure:
 *
 * <blockquote>
 *     <TOPICS><D>class1</D><D>class2</D>...</TOPICS>
 * </blockquote>
 *
 * @param is Input stream from which the next topic list will be extracted.
 *
 * @return std::vector of topics the document belongs to. If there are no
 * topics, the returned vector is empty.
 */
std::vector<ir::DocClass> get_doc_topics(std::istream& is) {
    std::vector<ir::DocClass> result;

    std::string line;
    size_t beg_header_pos;
    while (std::getline(is, line)) {
        // advance until topic header beginning is found
        beg_header_pos = line.find(ir::TOPIC_HEADER_BEG);
        if (beg_header_pos != std::string::npos) {
            // find class beginning index
            size_t class_tag_beg_pos = line.find(ir::CLASS_BEG_TAG);

            // iterate over all class tags
            while (class_tag_beg_pos != std::string::npos) {
                size_t class_tag_end_pos =
                    line.find(ir::CLASS_END_TAG, class_tag_beg_pos);

                // current class tag indices
                size_t class_beg = class_tag_beg_pos + ir::CLASS_BEG_TAG.size();
                size_t class_len = class_tag_end_pos - class_beg;

                // get document class
                std::string doc_class = line.substr(class_beg, class_len);
                assert(!doc_class.empty());

                // switch the class
                if (doc_class == ir::EARN_CLASS_KEY) {
                    result.push_back(ir::DocClass::Earn);
                } else if (doc_class == ir::ACQ_CLASS_KEY) {
                    result.push_back(ir::DocClass::Acq);
                } else if (doc_class == ir::MONEY_FX_CLASS_KEY) {
                    result.push_back(ir::DocClass::MoneyFx);
                } else if (doc_class == ir::GRAIN_CLASS_KEY) {
                    result.push_back(ir::DocClass::Grain);
                } else if (doc_class == ir::CRUDE_CLASS_KEY) {
                    result.push_back(ir::DocClass::Crude);
                } else {
                    result.push_back(ir::DocClass::Other);
                }

                // go to the next class tag
                class_tag_beg_pos =
                    line.find(ir::CLASS_BEG_TAG, class_tag_end_pos);
            }

            return result;
        }
    }
    // we should never come here if the input is proper
    throw std::invalid_argument("Input stream does not contain " +
                                ir::TOPIC_HEADER_BEG + " and " +
                                ir::TOPIC_HEADER_END + " tags");
}

std::tuple<ir::raw_doc_index, ir::doc_type_index, ir::doc_multiclass_index >
ir::parse_file(std::istream& ifs) {
    raw_doc_index docs;
    doc_type_index doc_types;
    doc_multiclass_index doc_classes;
    std::string line;

    int id;
    DocType type;
    raw_doc doc;
    while (std::getline(ifs, line)) {
        if (line.find(DOC_HEADER) == 0) {
            // found a new document
            id = get_doc_id(line);
            type = get_doc_type(line);

            // get document topics
            auto topics = get_doc_topics(ifs);

            // get document string
            doc = get_next_doc(ifs);
            doc = text_between_tags(doc, TITLE_BEG_TAG, TITLE_END_TAG) + "\n" +
                  text_between_tags(doc, BODY_BEG_TAG, BODY_END_TAG);

            // document string
            docs[id] = doc;
            // document type (train/test)
            doc_types[id] = type;
            // document topics
            doc_classes[id] = topics;
        }
    }

    return std::make_tuple(docs, doc_types, doc_classes);
};
