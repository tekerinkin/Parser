#include "../include/Statement.h"
#include <iostream>

namespace simpleparser {

    void Statement::debugPrint(size_t indent) {
        cout << string(indent, '\t') << sStatementKindNames[int(_kind)] << " ";
        cout << _type._name << " " << _name << " (\n";

        for(auto statement : _parameters)
            statement.debugPrint(indent + 1);

        cout << string(indent, '\t') << ")" << endl;
    }
}
