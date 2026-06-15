#pragma once

#include "../runtime_context.hpp"

class InternalHttpServer
{
public:
  virtual ~InternalHttpServer() = default;

  virtual void listen(int port, RuntimeContext context) = 0;
};
