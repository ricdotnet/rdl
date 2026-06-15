#pragma once

#include "../interpreter.hpp"
#include "../runtime_context.hpp"

NativeFn marshal_lambda(RuntimeContext context);

NativeFn unmarshal_lambda(RuntimeContext context);
