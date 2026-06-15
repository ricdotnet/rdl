#pragma once

#include "./native_module.hpp"
#include "../runtime_context.hpp"

class JsonModule : NativeModule
{
private:
  RuntimeContext context;

public:
  explicit JsonModule(const RuntimeContext context) : NativeModule(), context(context) {}

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] Value init() override;
};
