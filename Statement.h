#pragma once

#include <string>
#include <vector>
#include "Type.h"

namespace simpleparser {

    using namespace std;

    enum class StatementKind: int {
        VARIABLE_DECLARATION,
        FUNCTION_CALL,
        LITERAL,
        OPERATOR_CALL,
        VARIABLE_NAME,
        WHILE_LOOP
    };

    static const char* sStatementKindNames[] = {
            "VARIABLE_DECLARATION",
            "FUNCTION_CALL",
            "LITERAL",
            "OPERATOR_CALL",
            "VARIABLE_NAME",
            "WHILE_LOOP"
    };

    class Statement {
    public:
        string _name;
        Type _type{Type("void", VOID)};
        vector<Statement> _parameters;
        StatementKind _kind{StatementKind::FUNCTION_CALL};

        void debugPrint(size_t indent);
    };
}