//
// Created by Дима Шестаков on 25.06.2022.
//

#include "../include/Tokenizer.hpp"
#include <stdexcept>
#include <iostream>

namespace simpleparser {
    vector<Token> Tokenizer::parse(const string &inProgram) {
        vector<Token> tokens;
        Token currentToken;

        for (char curr: inProgram) {
            if (currentToken._type == STRING_ESCAPE_SEQUENCE) {
                switch (curr) {
                    case 'n':
                        currentToken._text.append(1, '\n');
                        break;
                    case 'r':
                        currentToken._text.append(1, '\r');
                        break;
                    case 't':
                        currentToken._text.append(1, '\t');
                        break;
                    case '\\':
                        currentToken._text.append(1, '\\');
                        break;
                    default:
                        throw runtime_error(string("unknown escape sequence: \\") + string(1, curr) +
                                            "in string on line " + to_string(currentToken._lineNumber) + ".");
                }
                currentToken._type = STRING_LITERAL;
                continue;
            } else if(currentToken._type == POTENTIAL_COMMENT && curr != '/') {
                currentToken._type = OPERATOR;
                endToken(currentToken, tokens);
                continue;
            }

            switch (curr) {
                case '0' ... '9':
                    if (currentToken._type == WHITESPACE) {
                        currentToken._type = INTEGER_LITERAL;
                        currentToken._text.append(1, curr);
                    } else if(currentToken._type == POTENTIAL_DOUBLE) {
                        currentToken._type = DOUBLE_LITERAL;
                        currentToken._text.append(1, curr);
                    } else {
                        currentToken._text.append(1, curr);
                    }
                    break;

                case '.':
                    if (currentToken._type == WHITESPACE) {
                        currentToken._type = POTENTIAL_DOUBLE;
                        currentToken._text.append(1, curr);
                    } else if(currentToken._type == INTEGER_LITERAL) {
                        currentToken._type = DOUBLE_LITERAL;
                        currentToken._text.append(1, curr);
                    } else if(currentToken._type == STRING_LITERAL) {
                        currentToken._text.append(1, curr);
                    } else {
                        endToken(currentToken, tokens);
                        currentToken._type = OPERATOR;
                        currentToken._text.append(1, curr);
                        endToken(currentToken, tokens);
                    }
                    break;

                case '{':
                case '}':
                case '(':
                case ')':
                case '=':
                case '-':
                case ';':
                case ',':
                    if (currentToken._type != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken._type = OPERATOR;
                        currentToken._text.append(1, curr);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken._text.append(1, curr);
                    }
                    break;

                case ' ':
                case '\t':
                    if(currentToken._type == STRING_LITERAL || currentToken._type == COMMENT) {
                        currentToken._text.append(1, curr);
                    } else {
                        endToken(currentToken, tokens);
                    }
                    break;
                case '\r':
                case '\n':
                    endToken(currentToken, tokens);
                    ++currentToken._lineNumber;
                    break;

                case '"':
                    if (currentToken._type != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken._type = STRING_LITERAL;
                    } else if (currentToken._type == STRING_LITERAL) {
                        endToken(currentToken, tokens);
                    }
                    break;

                case '\\':
                    if (currentToken._type == STRING_LITERAL) {
                        currentToken._type = STRING_ESCAPE_SEQUENCE;
                    } else {
                        endToken(currentToken, tokens);
                        currentToken._type = OPERATOR;
                        currentToken._text.append(1, curr);
                        endToken(currentToken, tokens);
                    }
                    break;

                case '/':
                    if(currentToken._type == STRING_LITERAL) {
                        currentToken._text.append(1, curr);
                    } else if(currentToken._type == POTENTIAL_COMMENT) {
                        currentToken._type = COMMENT;
                        currentToken._text.erase();
                    } else {
                        currentToken._type = POTENTIAL_COMMENT;
                        currentToken._text.append(1, curr);
                    }

                default:
                    if(currentToken._type == WHITESPACE || currentToken._type == INTEGER_LITERAL
                                                        || currentToken._type == DOUBLE_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken._type = IDENTIFIER;
                        currentToken._text.append(1, curr);
                    } else {
                        currentToken._text.append(1, curr);
                    }
                    break;
            }

        }

        endToken(currentToken, tokens);

        return tokens;
    }

    void Tokenizer::endToken(Token &token, vector<Token> &tokens) {
        if (token._type == COMMENT) {
            cout << "Ignoring comment " << token._text << endl;
        } else if(token._type != WHITESPACE) {
            tokens.push_back(token);
        }
        if(token._type == POTENTIAL_DOUBLE) {
            if(token._text.compare(".") == 0) {
                token._type = OPERATOR;
            } else {
                token._type = DOUBLE_LITERAL;
            }
        }
        token._type = WHITESPACE;
        token._text.erase();
    }

    void Token::debugPrint() const {
        cout << "Token(" << sTokenTypeStrings[_type] << ", \"" << _text;
        cout << "\", " << _lineNumber << ")" << endl;
    }
}

