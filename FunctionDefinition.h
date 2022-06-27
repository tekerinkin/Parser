#pragma once

#include <string>
#include <vector>
#include "Type.h"
#include "Statement.h"

namespace simpleparser {

    using namespace std;

    class ParameterDefinition{
    public:
        string _parameterName;
        Type _parameterType;

        void debugPrint(size_t indent) const;
    };

    class FunctionDefinition{
    public:
        string _name;
        vector<ParameterDefinition> _parameters;
        vector<Statement> _statements;
        bool _returnsSmth;
        Type _type;

        void debugPrint() const;
    };
}