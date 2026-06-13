#pragma once

#include "../interpreter.hpp"
#include "../libs/json.hpp"

inline Value unmarshall(const nlohmann::json &, const ValueType &, const RuntimeContext )
{

  return Value::nil_value();
}
