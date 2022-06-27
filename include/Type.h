#pragma once

#include <string>
#include <vector>

namespace simpleparser {

    using namespace std;

    enum BUILTIN_TYPE {
        INT8,
        UINT8,
        INT32,
        UINT32,
        DOUBLE,
        STRUCT,
        VOID
    };

    class Type {
    public:
        Type(const string &name = "", enum BUILTIN_TYPE type = VOID) :
                _name(name), _type(type) {}

        string _name;
        enum BUILTIN_TYPE _type;
        vector<Type> _field; // for structs
    };
}