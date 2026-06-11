#pragma once

#include "./native_module.hpp"

class TimeModule : NativeModule
{
public:
  explicit TimeModule() : NativeModule() {}

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] Value init() override;
};
