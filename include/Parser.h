#pragma once

#include "Tokenizer.hpp"
#include "Type.h"
#include "FunctionDefinition.h"
#include <optional>
#include <map>

namespace simpleparser{

    class Parser {
    public:
        Parser();


        void parse(vector<Token>& tokens);

        void debugPrint() const;
    private:
        optional<Type> expectType();

        optional<Token> expectIdentifier(const string& name = string());
        optional<Token> expectOperator(const string& name = string());

        bool expectFunctionDefinition();

        optional<vector<Statement>> parseFunctionBody();

        optional<Statement> expectOneValue();

        optional<Statement> expectWhileLoop();

        optional<Statement> expectStatement();

        optional<Statement> expectVariableDecl();

        optional<Statement> expectFunctionCall();

        optional<Statement> expectExpression();

        size_t operatorPrecedence(const string& operatorName);

        Statement* findRightMostStatement(Statement* lhs, size_t rhsPrecedence);

    private:
        vector<Token>::iterator _currentToken;
        vector<Token>::iterator _endToken;
        map<string, Type> _types;
        map<string, FunctionDefinition> _functions;
    };
}



