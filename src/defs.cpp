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

#include "defs.hpp"
#include <iostream>

std::string ir::to_string(ir::DocClass doc_class) {
    switch (doc_class) {
        case ir::DocClass::Earn:
            return "earn";
        case ir::DocClass::Acq:
            return "acq";
        case ir::DocClass::MoneyFx:
            return "money-fx";
        case ir::DocClass::Grain:
            return "grain";
        case ir::DocClass::Crude:
            return "crude";
        case ir::DocClass::Other:
            return "other";
    }
}

std::ostream& ir::operator<<(std::ostream& os, ir::DocClass doc_class) {
    os << ir::to_string(doc_class);
    return os;
}

std::istream& ir::operator>>(std::istream& is, DocClass& doc_class) {
    std::string class_str;
    is >> class_str;
    if (class_str == "earn") {
        doc_class = DocClass::Earn;
    } else if (class_str == "acq") {
        doc_class = DocClass::Acq;
    } else if (class_str == "money-fx") {
        doc_class = DocClass::MoneyFx;
    } else if (class_str == "grain") {
        doc_class = DocClass::Grain;
    } else if (class_str == "crude") {
        doc_class = DocClass::Crude;
    } else {
        doc_class = DocClass::Other;
    }

    return is;
}
