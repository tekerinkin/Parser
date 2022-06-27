cmake_minimum_required(VERSION 3.21)
project(Parser)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE
        include/Tokenizer.hpp
        include/Parser.h
        include/FunctionDefinition.h
        include/Type.h
        include/Statement.h)

set(SRC
        src/Tokenizer.cpp
        src/Parser.cpp
        src/FunctionDefinition.cpp
        src/Statement.cpp)

add_library(Parser_internals
            ${INCLUDE}
            ${SRC})

add_executable(Parser src/main.cpp)

target_link_libraries(Parser Parser_internals)