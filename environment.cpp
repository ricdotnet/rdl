#include "environment.hpp"
#include "interpreter.hpp"

void Environment::set(const std::string &name, const Value &value) {
    values[name] = value;
}

Value Environment::get(const std::string &name) {
    if (!values.contains(name)) {
        return Value::undefined_value();
    }

    return values.at(name);
}
