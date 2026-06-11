#include "./http_server.hpp"
#include "../interpreter.cpp"
#include "../libs/httplib.h"

class HttpServer : public InternalHttpServer
{
public:
  void listen(const int port, const RuntimeContext context) override
  {
    httplib::Server server;

    server.Get(R"(.*)", [&](const httplib::Request &req, httplib::Response &res) {
      const auto start = std::chrono::system_clock::now();

      const auto route = context.runtime->find_route(req.method, req.path);

      if (!route)
      {
        res.status = 404;
        return;
      }

      const auto response_handle = std::make_shared<ResponseHandle>();
      response_handle->response = &res;

      const auto response_props = std::make_shared<std::unordered_map<std::string, Value> >();
      auto response_object = Value::object_value(response_props);

      response_object.object.native_object = response_handle;
      register_response_methods(response_props.get());

      // register_request_methods(request_props.get());

      *context.runtime->out << "[" << req.method << "] " << req.path << std::endl;

      Interpreter interpreter(context);
      interpreter.execute_route(*route, Value::nil_value(), response_object);

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
};
