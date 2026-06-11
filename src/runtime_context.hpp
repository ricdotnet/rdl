#pragma once

#include "./runtime.hpp"

struct RuntimeContext
{
  Environment *environment;

  Runtime *runtime;
};
