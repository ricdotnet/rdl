#pragma once

#include "./native_module.hpp"

class FileSystemModule : NativeModule
{
public:
  explicit FileSystemModule() : NativeModule() {}

  [[nodiscard]] std::string name() const override;

  [[nodiscard]] Value init() override;
};
