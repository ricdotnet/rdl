#pragma once

#include "interpreter.hpp"

#include <string>
#include <unordered_map>

class Environment {
public:
    std::unordered_map<std::string, Value> values;

    void set(const std::string &name, const Value &value);

    Value get(const std::string &name);
};
