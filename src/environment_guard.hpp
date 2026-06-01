#pragma once

#include "./environment.hpp"

class EnvironmentGuard
{
  Environment *&current;

  Environment *previous;

public:
  EnvironmentGuard(Environment *&current, Environment *next) : current(current), previous(current)
  {
    current = next;
  }

  ~EnvironmentGuard()
  {
    current = previous;
  }
};
