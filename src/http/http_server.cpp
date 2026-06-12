#include "./http_server.hpp"
#include "../interpreter.cpp"
#include "../libs/httplib.h"

class HttpServer : public InternalHttpServer
{
private:
  static Value request_handle(const httplib::Request &req)
  {
    const auto request_handle = std::make_shared<RequestHandle>();
    request_handle->request = &req;
    const auto request_props = std::make_shared<std::unordered_map<std::string, Value> >();

    request_props->insert({"method", Value::string_value(req.method)});
    request_props->insert({"path", Value::string_value(req.path)});

    auto request_object = Value::object_value(request_props);
    request_object.object.native_object = request_handle;
    register_request_methods(request_props.get());

    return request_object;
  }

  static Value response_handle(httplib::Response &res)
  {
    const auto response_handle = std::make_shared<ResponseHandle>();
    response_handle->response = &res;
    const auto response_props = std::make_shared<std::unordered_map<std::string, Value> >();
    auto response_object = Value::object_value(response_props);
    response_object.object.native_object = response_handle;
    register_response_methods(response_props.get());

    return response_object;
  }

public:
  void listen(const int port, const RuntimeContext context) override
  {
    httplib::Server server;

    server.Get(R"(.*)", [&](const httplib::Request &req, httplib::Response &res) {
      const auto start = std::chrono::system_clock::now();

      const auto route = context.runtime->find_route(req.method, req.path);

      if (!route.body)
      {
        res.status = 404;
        return;
      }

      *context.runtime->out << "[" << req.method << "] " << req.path << std::endl;

      Interpreter interpreter(context);
      interpreter.execute_route(route, request_handle(req), response_handle(res));

      const auto end = std::chrono::system_clock::now();
      const auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      *context.runtime->out << "[" << req.method << "] " << req.path << " " << duration_microseconds.count() << "μs" <<
          "\n";
    });

    *context.runtime->out << "Server listening on port " << port << std::endl;

    server.listen("0.0.0.0", port);
  }

  static void register_response_methods(std::unordered_map<std::string, Value> *properties)
  {
    (*properties)["send"] = Value::builtin_function_value(
      [](const Value &receiver, const std::vector<Value> &args) -> Value {
        const auto response = std::static_pointer_cast<ResponseHandle>(receiver.object.native_object);

        response->response->status = 200;
        const std::string body = args.empty() ? "" : args[0].to_string();
        response->response->set_content(body, "text/plain");

        return Value::nil_value();
      });
  }

  static void register_request_methods(std::unordered_map<std::string, Value> *properties)
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
  }
};
