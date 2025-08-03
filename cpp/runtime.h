#pragma once

#include "error.h"
#include "result.h"

namespace kero {

class Runtime {
public:
  Result<void, Error> run() noexcept;
};

} // namespace kero
