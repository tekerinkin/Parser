#pragma once

#include <vector>
#include <string>

namespace simpleparser {

    using namespace std;

    enum TokenType {
        WHITESPACE,
        IDENTIFIER,
        INTEGER_LITERAL,
        DOUBLE_LITERAL,
        STRING_LITERAL,
        OPERATOR,
        STRING_ESCAPE_SEQUENCE,
        POTENTIAL_DOUBLE,
        POTENTIAL_COMMENT,
        COMMENT
    };

    static const char *sTokenTypeStrings[] = {
            "WHITESPACE",
            "IDENTIFIER",
            "INTEGER_LITERAL",
            "DOUBLE_LITERAL",
            "STRING_LITERAL",
            "OPERATOR",
            "STRING_ESCAPE_SEQUENCE",
            "POTENTIAL_DOUBLE",
            "POTENTIAL_COMMENT",
            "COMMENT"
    };

    class Token {
    public:
        enum TokenType _type{WHITESPACE};
        string _text;
        size_t _lineNumber{0};

        void debugPrint() const;
    };

    class Tokenizer {
    public:
        vector<Token> parse(const string &inProgram);

    private:
        // @brief Add separate tokens to vector
        void endToken(Token &token, vector<Token> &tokens);
    };
}

