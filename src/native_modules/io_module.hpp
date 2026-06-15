#pragma once

#include "./native_module.hpp"
#include "../runtime_context.hpp"

class IoModule : public NativeModule
{
private:
  RuntimeContext context;

public:
  explicit IoModule(const RuntimeContext context) : NativeModule(), context(context) {}

  ~IoModule() override = default;

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] Value init() override;
};
