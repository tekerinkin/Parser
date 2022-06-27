#include "../include/Parser.h"
#include <iostream>

namespace simpleparser {

    struct OperatorEntry {
        string _name;
        size_t _precedence;
    };

    static map<string, OperatorEntry> sOperators {
            {"=", OperatorEntry{"=", 1}},
            {"<", OperatorEntry{"<", 5}},
            {"+", OperatorEntry{"+", 10}},
            {"-", OperatorEntry{"-", 10}},
            {"/", OperatorEntry{"/", 50}},
            {"*", OperatorEntry{"*", 50}}
    };

    Parser::Parser() {
        _types["int"] = Type("signed int", INT32);
        _types["unsigned"] = Type("unsigned int", UINT32);
        _types["char"] = Type("signed char", INT8);
        _types["uint8_t"] = Type("uint8_t", UINT8);
        _types["void"] = Type("void", VOID);
        _types["double"] = Type("double", DOUBLE);
    }

    bool Parser::expectFunctionDefinition() {
        auto parseStart = _currentToken;
        optional<Type> possibleType = expectType();
        if(possibleType.has_value()) {
            optional<Token> possibleName = expectIdentifier();

            if(possibleName.has_value()) {
                optional<Token> possibleOperator = expectOperator("(");

                if(possibleOperator.has_value()) {

                    FunctionDefinition func;
                    func._name = possibleName->_text;
                    func._type = Type(possibleType->_name, possibleType->_type);

                    cout << "We have a function: " << possibleName->_text << ".";

                    while(!expectOperator(")").has_value()) {
                        optional<Type> possibleParamType = expectType();

                        if(!possibleParamType.has_value())
                            throw runtime_error("Expected a type at start of arg's list");

                        optional<Token> possibleVarName = expectIdentifier();

                        ParameterDefinition param;
                        param._parameterType = possibleParamType->_name;

                        if(possibleVarName.has_value()) {
                            param._parameterName = possibleVarName->_text;
                        }

                        func._parameters.push_back(param);

                        if(expectOperator(")").has_value())
                            break;

                        if(!expectOperator(",").has_value())
                            throw runtime_error("Expected ',' to separate parameters or ')' to indicate end of argument list.");
                    }

                    auto statements = parseFunctionBody();

                    if(!statements.has_value()) {
                        _currentToken = parseStart;
                        return false;
                    }
                    func._statements.insert(func._statements.begin(),
                                            statements->begin(), statements->end());

                    _functions[func._name] = func;

                    return true;
                } else {
                    _currentToken = parseStart;
                }
            } else {
                _currentToken = parseStart;
            }
        }
        return false;
    }

    void Parser::parse(vector<Token>& tokens){
        _endToken = tokens.end();
        _currentToken = tokens.begin();

        while(_currentToken != _endToken) {
            if(expectFunctionDefinition()) {

            } else {
                cerr << "Unknown identifier " << _currentToken->_text << "." << endl;
                _currentToken++;
            }
        }
    }

    optional<Token> Parser::expectIdentifier(const string &name) {
        if(_currentToken == _endToken)
            return nullopt;

        if(_currentToken->_type != IDENTIFIER)
            return nullopt;

        if(!name.empty() && _currentToken->_text != name)
            return nullopt;

        auto returnToken = *_currentToken;
        ++_currentToken;
        return returnToken;
    }

    optional<Token> Parser::expectOperator(const string &name) {
        if(_currentToken == _endToken)
            return nullopt;

        if(_currentToken->_type != OPERATOR)
            return nullopt;

        if(!name.empty() && _currentToken->_text != name)
            return nullopt;

        auto returnToken = *_currentToken;
        ++_currentToken;
        return returnToken;
    }

    optional<Type> Parser::expectType() {
        auto possibleType = expectIdentifier();
        if(!possibleType)
            return nullopt;
        auto foundType = _types.find(possibleType->_text);
        if(foundType == _types.end()) {
            _currentToken--;
            return nullopt;
        }


        return foundType->second;
    }

    optional<vector<Statement>> Parser::parseFunctionBody() {
        if(!expectOperator("{").has_value())
            return nullopt;

        vector<Statement> statements;

        while(!expectOperator("}").has_value()) {
            auto statement = expectStatement();
            if(statement.has_value())
                statements.push_back(statement.value());

            if(!expectOperator(";").has_value()) {
                size_t lineNo = (_currentToken != _endToken) ? _currentToken -> _lineNumber : 999999;
                throw runtime_error(string("Expected ';' at end of statement in line ")
                + to_string(lineNo) + ".");
            }
        }

        return statements;
    }

    void Parser::debugPrint() const {

        for(auto [_,func] : _functions)
            func.debugPrint();
    }

    optional<Statement> Parser::expectOneValue() {
        optional<Statement> res;

        auto savedToken = _currentToken;

        if(_currentToken != _endToken && _currentToken->_type == DOUBLE_LITERAL) {
            Statement dbLStatement;
            dbLStatement._kind = StatementKind::LITERAL;
            dbLStatement._name = _currentToken->_text;
            dbLStatement._type = Type("double", DOUBLE);
            res = dbLStatement;
            _currentToken++;
        } else if(_currentToken != _endToken && _currentToken->_type == STRING_LITERAL) {
            Statement strLStatement;
            strLStatement._kind = StatementKind::LITERAL;
            strLStatement._type = Type("string", UINT8);
            strLStatement._name = _currentToken->_text;
            res = strLStatement;
            _currentToken++;
        } else if(_currentToken != _endToken && _currentToken->_type == INTEGER_LITERAL) {
            Statement intLStatement;
            intLStatement._kind = StatementKind::LITERAL;
            intLStatement._name = _currentToken->_text;
            intLStatement._type = Type("signed integer", INT32);
            res = intLStatement;
            _currentToken++;
        } else if (expectOperator("(").has_value()) {
            res = expectExpression();
            if(!expectOperator(")").has_value())
                throw runtime_error("Unbalanced '(' in parenthesized expression");

        } else if(auto varName = expectIdentifier()) {
            if(expectOperator("("))
                _currentToken = savedToken;
            else {
                Statement varNameStatement;
                varNameStatement._kind = StatementKind::VARIABLE_NAME;
                varNameStatement._name = _currentToken->_text;
                res = varNameStatement;
            }
        }
        if(!res.has_value())
            res = expectFunctionCall();

        return res;
    }

    optional<Statement> Parser::expectVariableDecl() {
        auto startToken = _currentToken;
        optional<Type> possibleType = expectType();
        if(!possibleType.has_value()) {
            _currentToken = startToken;
            return nullopt;
        }

        optional<Token> possibleVarName = expectIdentifier();
        if(!possibleVarName.has_value()) {
            _currentToken = startToken;
            return nullopt;
        }

        Statement statement;

        statement._kind = StatementKind::VARIABLE_DECLARATION;
        statement._name = _currentToken->_text;
        statement._type = possibleType.value();

        if(expectOperator("=").has_value()) {
            optional<Statement> initialValue = expectExpression();
            if(!initialValue.has_value())
                throw runtime_error("Expected initial value to right of '=' in variable declaration");

            statement._parameters.push_back(initialValue.value());
        }

        return statement;
    }

    optional<Statement> Parser::expectFunctionCall() {
        auto startToken = _currentToken;

        optional<Token> possibleFuncName = expectIdentifier();
        if(!possibleFuncName.has_value() || !expectOperator("(").has_value()) {
            _currentToken = startToken;
            return nullopt;
        }

        Statement functionCall;
        functionCall._kind = StatementKind::FUNCTION_CALL;
        functionCall._name = _currentToken->_text;

        while(!expectOperator(")").has_value()) {
            optional<Statement> param = expectExpression();
            if(!param.has_value())
                throw runtime_error("Expected expression as a parameter");

            functionCall._parameters.push_back(param.value());

            if(expectOperator(")").has_value())
                break;
            if(!expectOperator(",").has_value())
                throw runtime_error(string("Expected ',' to separate parameters, found ") + _currentToken->_text);
        }

        return functionCall;
    }

    optional<Statement> Parser::expectWhileLoop() {
        Statement whileLoop{"", Type{"void", VOID},
                            {}, StatementKind::WHILE_LOOP};

        size_t lineNo = (_currentToken != _endToken) ? _currentToken->_lineNumber : 999999;
        if(!expectIdentifier("while").has_value())
            return nullopt;

        if(!expectOperator("(").has_value())
            throw runtime_error(string("Expected opening parenthesis after \"while\" on line ")
            + to_string(lineNo));

        if(_currentToken != _endToken)
            lineNo = _currentToken->_lineNumber;

        optional<Statement> condition = expectExpression();

        if(!condition.has_value())
            throw runtime_error(string("Expected loop condition after \"while\" statement on line ")
            + to_string(lineNo));

        whileLoop._parameters.push_back(condition.value());

        if(!expectOperator(")").has_value())
            throw runtime_error(string("Expected closing parenthesis after \"while\" condition on line ")
            + to_string(lineNo));

        if(!expectOperator("{").has_value())
            throw runtime_error(string("Expected opening curly bracket after \"while\" condition on line ")
            + to_string(lineNo));

        while(_currentToken != _endToken && !expectOperator(")").has_value()) {
            auto currStatement = expectStatement();
            if(!currStatement.has_value())
                break;
            whileLoop._parameters.push_back(currStatement.value());

            if(!expectOperator(";").has_value()) {
                size_t lineNo = (_currentToken != _endToken) ? _currentToken->_lineNumber : 999999;
                throw runtime_error(string("Expected ';' at end of statement in line ") + to_string(lineNo));
            }
        }
        return whileLoop;
    }

    optional<Statement> Parser::expectStatement() {
        optional<Statement> res = expectWhileLoop();

        if(!res.has_value())
            res = expectVariableDecl();
        if(!res.has_value())
            res = expectExpression();

        return res;

    }

    optional<Statement> Parser::expectExpression() {
        optional<Statement> lhs = expectOneValue();
        if(!lhs.has_value())
            return nullopt;

        while(true) {
            optional<Token> op = expectOperator();
            if(!op.has_value())
                break;

            int rhsPrecedence = operatorPrecedence(op->_text);
            if(rhsPrecedence == 0) {
                _currentToken--;
                return lhs;
            }

            optional<Statement> rhs = expectOneValue();
            if(!rhs.has_value()) {
                _currentToken--;
                return lhs;
            }

            Statement* rightMostStatement = findRightMostStatement(&lhs.value(), rhsPrecedence);
            if(rightMostStatement) {
                Statement operatorCall;
                operatorCall._kind = StatementKind::OPERATOR_CALL;
                operatorCall._name = op->_text;
                operatorCall._parameters.push_back(rightMostStatement->_parameters.at(1));
                operatorCall._parameters.push_back(rhs.value());
                rightMostStatement->_parameters[1] = operatorCall;
            } else {
                Statement operatorCall;
                operatorCall._kind = StatementKind::OPERATOR_CALL;
                operatorCall._name = op->_text;
                operatorCall._parameters.push_back(lhs.value());
                operatorCall._parameters.push_back(rhs.value());
                lhs = operatorCall;
            }
        }
        return lhs;
    }

    Statement* Parser::findRightMostStatement(Statement *lhs, size_t rhsPrecedence) {
        if(lhs->_kind != StatementKind::OPERATOR_CALL)
            return nullptr;
        if(operatorPrecedence(lhs->_name) >= rhsPrecedence)
            return nullptr;

        Statement* rhs = &lhs->_parameters.at(1);
        rhs = findRightMostStatement(rhs, rhsPrecedence);
        if(!rhs)
            return lhs;
        return rhs;
    }

    size_t Parser::operatorPrecedence(const string &operatorName) {
        auto foundOp = sOperators.find(operatorName);

        return foundOp != sOperators.end() ? foundOp->second._precedence : 0;
    }


}