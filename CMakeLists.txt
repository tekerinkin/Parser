cmake_minimum_required(VERSION 3.21)
project(Parser)

set(CMAKE_CXX_STANDARD 17)

add_library(Parser_internals
        Tokenizer.cpp
        Tokenizer.hpp
        Parser.cpp
        Parser.h FunctionDefinition.cpp FunctionDefinition.h Type.h Statement.cpp Statement.h)

add_executable(Parser main.cpp)

target_link_libraries(Parser Parser_internals)