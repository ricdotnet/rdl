#pragma once

#include "../environment.hpp"
#include "../interpreter.hpp"
#include "../libs/json.hpp"

inline Value unmarshal(const nlohmann::json &json, const TypeDescriptor &type, const RuntimeContext context)
{
  switch (type.type)
  {
    case ValueType::String:
      return Value::string_value(json.get<std::string>());
    case ValueType::Number:
      return Value::number_value(json.get<ssize_t>());
    case ValueType::Boolean:
      return Value::boolean_value(json.get<bool>());
    case ValueType::Struct: {
      const auto definition = context.environment->get(type.name);
      const auto fields = std::make_shared<std::unordered_map<std::string, Value> >();

      for (auto &[field_name, field_type]: definition.struct_definition->fields)
      {
        (*fields)[field_name] = unmarshal(json.at(field_name), field_type.type, context);
      }

      return Value::struct_instance_value(definition.struct_definition, fields);
    }
    case ValueType::Array: {
      std::vector<Value> elements;

      for (auto &item: json)
      {
        elements.push_back(unmarshal(item, *type.element_type, context));
      }

      return Value::array_value(type.element_type->type, std::make_shared<std::vector<Value> >(std::move(elements)));
    }
    default:
      ErrorService::runtime_error("Cannot unmarshal payload", "");
      return Value::nil_value();
  }
}

inline nlohmann::json marshal(const Value &value, const RuntimeContext context)
{
  switch (value.type)
  {
    case ValueType::String:
      return value.string;

    case ValueType::Number:
      return value.number;

    case ValueType::Boolean:
      return value.boolean;

    case ValueType::Nil:
      return nullptr;

    case ValueType::Struct: {
      nlohmann::json object = nlohmann::json::object();

      const auto definition = value.struct_instance.definition;
      const auto fields = value.struct_instance.fields;

      for (const auto &[field_name, field_definition]: definition->fields)
      {
        const auto json_name = field_definition.json_name.value_or(field_name);

        object[json_name] = marshal(fields->at(field_name), context);
      }

      return object;
    }

    case ValueType::Array: {
      nlohmann::json array = nlohmann::json::array();

      for (const auto &element: *value.array.elements)
      {
        array.push_back(marshal(element, context));
      }

      return array;
    }

    case ValueType::Object: {
      nlohmann::json object = nlohmann::json::object();

      for (const auto &[key, val]: *value.object.properties)
      {
        object[key] = marshal(val, context);
      }

      return object;
    }

    default:
      ErrorService::runtime_error("Cannot marshal value of type", Value::type_name(value.type));

      return nullptr;
  }
}
