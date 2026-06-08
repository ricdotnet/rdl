#pragma once

#include "../interpreter.hpp"
#include "../runtime.hpp"

class NativeModule
{
public:
  explicit NativeModule() = default;

  virtual ~NativeModule() = default;

  [[nodiscard]] virtual std::string name() const = 0;

  virtual Value init() = 0;
};
