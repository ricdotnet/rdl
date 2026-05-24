#include "environment.hpp"

#include "interpreter.hpp"

void Environment::define(const std::string &name, const Value &value) {
    if (values.contains(name)) {
        throw std::runtime_error("Variable already defined: " + name);
    }

    values[name] = {value, false};
}

Value Environment::get(const std::string &name) {
    if (!values.contains(name)) {
        return Value::undefined_value();
    }

    return values.at(name).value;
}

void Environment::assign(const std::string &name, const Value &value) {
    const auto current = values.find(name);

    if (current == values.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }

    if (!current->second.is_mutable) {
        throw std::runtime_error("Cannot reassign to immutable variable: " + name);
    }

    current->second.value = value;
}
