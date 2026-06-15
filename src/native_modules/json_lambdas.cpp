#include "./json_lambdas.hpp"
#include "../runtime_context.hpp"
#include "../http/utils.hpp"
#include "../libs/json.hpp"

NativeFn marshal_lambda(const RuntimeContext context)
{
  return [context](const Value &, const std::vector<Value> &args) {
    const auto &data = args[0];

    const auto marshalled_data = marshal(data, context);

    return Value::string_value(marshalled_data.dump());
  };
}


NativeFn unmarshal_lambda(const RuntimeContext context)
{
  return [context](const Value &, const std::vector<Value> &args) {
    const auto json_content = nlohmann::json::parse(args[0].string);
    const auto struct_def = args[1].struct_definition;

    const auto fields = std::make_shared<std::unordered_map<std::string, Value> >();

    for (const auto &[field_name, field_def]: struct_def->fields)
    {
      const auto field = &field_def;
      auto final_field_name = field->json_name.value_or(field_name);
      auto &value = json_content.at(final_field_name);

      const auto type = field->type;
      (*fields)[field_name] = unmarshal(value, type, context);
    }

    return Value::struct_instance_value(struct_def, fields);
  };
}
