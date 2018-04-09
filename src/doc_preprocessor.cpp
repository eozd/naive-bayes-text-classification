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

#include "doc_preprocessor.hpp"

/**
 * @brief Return a static map containing special HTML sequences and their ASCII
 * equivalents.
 *
 * @return std::unordered_map from HTML sequences to chars.
 */
static std::unordered_map<std::string, char> html_special_str_to_ascii() {
    return std::unordered_map<std::string, char>({
        {"&#1;", ' '},
        {"&#2;", ' '},
        {"&#3;", ' '},
        {"&#5;", 5},
        {"&#22;", ' '},
        {"&#27;", ' '},
        {"&#30;", 30},
        {"&#31;", 31},
        {"&#127;", ' '},
        {"&amp;", '&'},
        {"&lt;", '<'},
        {"&gt;", '>'},
    });
};

void ir::convert_html_special_chars(raw_doc& doc) {
    size_t index;
    for (const auto& sym_pair : html_special_str_to_ascii()) {
        const auto& html_sym = sym_pair.first;
        char ascii_char = sym_pair.second;

        // find all occurrences of html_sym
        while ((index = doc.find(html_sym)) != std::string::npos) {
            // replace the beginning with space
            for (size_t offset = 0; offset < html_sym.size() - 1; ++offset) {
                doc[index + offset] = ' ';
            }
            // replace last char with ASCII equivalent
            doc[index + html_sym.size() - 1] = ascii_char;
        }
    }
}
