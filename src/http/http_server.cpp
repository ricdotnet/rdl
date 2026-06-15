#include "./http_server.hpp"
#include "./utils.hpp"
#include "../interpreter.cpp"
#include "../libs/httplib.h"
#include "../libs/json.hpp"

class HttpServer : public InternalHttpServer
{
private:
  static Value request_handle(const RuntimeContext context, const httplib::Request &req)
  {
    const auto request_handle = std::make_shared<RequestHandle>();
    request_handle->request = &req;
    const auto request_props = std::make_shared<std::unordered_map<std::string, Value> >();

    request_props->insert({"method", Value::string_value(req.method)});
    request_props->insert({"path", Value::string_value(req.path)});

    auto request_object = Value::object_value(request_props);
    request_object.object.native_object = request_handle;
    register_request_methods(context, request_props.get());

    return request_object;
  }

  static Value response_handle(const RuntimeContext context, httplib::Response &res)
  {
    const auto response_handle = std::make_shared<ResponseHandle>();
    response_handle->response = &res;
    const auto response_props = std::make_shared<std::unordered_map<std::string, Value> >();
    auto response_object = Value::object_value(response_props);
    response_object.object.native_object = response_handle;
    register_response_methods(context, response_props.get());

    return response_object;
  }

  static httplib::Server::Handler handler_lambda(const RuntimeContext context)
  {
    return [context](const httplib::Request &req, httplib::Response &res) {
      const auto start = std::chrono::system_clock::now();

      const auto route = context.runtime->find_route(req.method, req.path);

      if (!route.body)
      {
        *context.runtime->out << "[" << req.method << "] " << req.path << " 404 Not Found" << std::endl;
        res.status = 404;
        return;
      }

      *context.runtime->out << "[" << req.method << "] " << req.path << std::endl;

      Interpreter interpreter(context);
      interpreter.execute_route(route, request_handle(context, req), response_handle(context, res));

      const auto end = std::chrono::system_clock::now();
      const auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      *context.runtime->out << "[" << req.method << "] " << req.path << " " << duration_microseconds.count() << "μs" <<
          "\n";
    };
  }

public:
  void listen(const int port, const RuntimeContext context) override
  {
    httplib::Server server;

    server.Get(R"(.*)", handler_lambda(context));
    server.Post(R"(.*)", handler_lambda(context));
    server.Put(R"(.*)", handler_lambda(context));
    server.Patch(R"(.*)", handler_lambda(context));
    server.Delete(R"(.*)", handler_lambda(context));
    server.Options(R"(.*)", handler_lambda(context));

    // TODO: websocket support later

    *context.runtime->out << "Server listening on port " << port << std::endl;

    server.listen("0.0.0.0", port);
  }

  static void register_response_methods(RuntimeContext context, std::unordered_map<std::string, Value> *properties)
  {
    (*properties)["send"] = Value::builtin_function_value(
      [context](const Value &receiver, const std::vector<Value> &args) -> Value {
        const auto response = std::static_pointer_cast<ResponseHandle>(receiver.object.native_object);

        if (response->finished)
        {
          ErrorService::runtime_error("Cannot send a response after the request is finished.", "");
        }

        response->finished = true;

        if (args.empty())
        {
          return Value::nil_value();
        }

        if (receiver.object.properties->contains("status"))
        {
          response->response->status = receiver.object.properties->at("status").number;
        }

        const auto response_body = args[0];

        if (response_body.type == ValueType::Object || response_body.type == ValueType::Array || response_body.type ==
            ValueType::Struct)
        {
          response->response->set_content(marshal(response_body, context).dump(), "application/json");
        }

        if (response_body.type == ValueType::String)
        {
          response->response->set_content(response_body.string, "text/plain");
        }

        return Value::nil_value();
      });
  }

  static void register_request_methods(RuntimeContext context, std::unordered_map<std::string, Value> *properties)
  {
    (*properties)["header"] = Value::builtin_function_value(
      [](const Value &receiver, const std::vector<Value> &args) -> Value {
        const auto request = std::static_pointer_cast<RequestHandle>(receiver.object.native_object);

        if (args.size() != 1)
        {
          return Value::nil_value();
        }

        const std::string header_name = args[0].to_string();
        const std::string header_value = request->request->get_header_value(header_name);

        return Value::string_value(header_value);
      });

    (*properties)["query"] = Value::builtin_function_value(
      [](const Value &receiver, const std::vector<Value> &args) -> Value {
        const auto request = std::static_pointer_cast<RequestHandle>(receiver.object.native_object);

        if (args.size() != 1)
        {
          return Value::nil_value();
        }

        const std::string param_name = args[0].to_string();
        const std::string param_value = request->request->get_param_value(param_name);

        return Value::string_value(param_value);
      });

    (*properties)["body"] = Value::builtin_function_value(
      [context](const Value &receiver, const std::vector<Value> &args) -> Value {
        const auto request = std::static_pointer_cast<RequestHandle>(receiver.object.native_object);

        if (args.size() != 1)
        {
          ErrorService::runtime_error("Expected 1 arguments for request body method.",
                                      "Found " + std::to_string(args.size()));
        }

        const auto struct_identifier = args[0].struct_definition->name;
        auto struct_def = context.environment->get(struct_identifier);

        if (struct_def.is_undefined)
        {
          ErrorService::runtime_error("Struct definition not found for identifier: ", struct_identifier);
        }

        if (struct_def.type != ValueType::Struct)
        {
          ErrorService::runtime_error("Expected struct definition for identifier: ", struct_identifier);
        }

        const auto parsed_body = nlohmann::json::parse(request->request->body);
        const auto fields = std::make_shared<std::unordered_map<std::string, Value> >();

        for (const auto &[field_name, field_def]: struct_def.struct_definition->fields)
        {
          auto field = &field_def;
          auto final_field_name = field->json_name.value_or(field_name);

          const auto value = parsed_body.at(final_field_name);

          const auto type = field->type;
          (*fields)[field_name] = unmarshal(value, type, context);
        }

        auto struct_definition = struct_def.struct_definition;
        return Value::struct_instance_value(struct_definition, fields);
      });
  }
};
