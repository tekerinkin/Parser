//
// Created by Дима Шестаков on 27.06.2022.
//

#include "../include/FunctionDefinition.h"
#include <iostream>


namespace simpleparser {
    void FunctionDefinition::debugPrint() const{
        cout << "\n" << _type._name  << " "<< _name << "(\n";

        for(auto param : _parameters)
            param.debugPrint(1);

        cout << ") {\n";
        for(auto statement : _statements)
            statement.debugPrint(0);

        cout << "}" << endl;
    }

    void ParameterDefinition::debugPrint(size_t indent) const{
        cout << string(indent, '\t')<< _parameterType._name << " " << _parameterName << endl;
    }


}