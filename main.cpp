#include "Tokenizer.hpp"
#include "Parser.h"
#include <iostream>

int main() {
    std::cout << "simpleparser 0.1\n" << std::endl;

    FILE* fh = fopen("/Users/dimashestakov/Desktop/Parser/test.myc", "r");
    fseek(fh, 0, SEEK_END);
    size_t fileSize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    std::string fileContents(fileSize, ' ');
    fread(fileContents.data(), 1, fileSize, fh);

    std::cout << fileContents << std::endl;

    simpleparser::Tokenizer tokenizer;
    std::vector<simpleparser::Token> tokens =
            tokenizer.parse(fileContents);

    for(auto curr : tokens) {
        curr.debugPrint();
    }

    simpleparser::Parser parser;
    parser.parse(tokens);

    parser.debugPrint();

    return 0;
}
