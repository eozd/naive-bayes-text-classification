#include "defs.hpp"

#include <iostream>

std::ostream& ir::operator<<(std::ostream& os, ir::DocClass doc_class) {
    switch (doc_class) {
        case ir::DocClass::Earn:
            os << "earn";
            break;
        case ir::DocClass::Acq:
            os << "acq";
            break;
        case ir::DocClass::MoneyFx:
            os << "money-fx";
            break;
        case ir::DocClass::Grain:
            os << "grain";
            break;
        case ir::DocClass::Crude:
            os << "crude";
            break;
        case ir::DocClass::Other:
            os << "other";
            break;
    }
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
