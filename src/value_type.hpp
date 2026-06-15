#pragma once

enum class ValueType
{
  Number,
  String,
  Boolean,
  Object,
  Nil,
  Undefined,
  Function,
  Range,
  Array,
  Struct,
};

struct TypeDescriptor
{
  ValueType type;

  std::string name;

  std::shared_ptr<TypeDescriptor> element_type;
};
