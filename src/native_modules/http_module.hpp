#pragma once

#include "./native_module.hpp"
#include "../runtime_context.hpp"

class HttpModule : NativeModule
{
private:
  RuntimeContext context;

public:
  explicit HttpModule(const RuntimeContext context) : NativeModule(), context(context) {}

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] Value init() override;
};
