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
