#pragma once

#include <iostream>

#ifdef _DEBUG
#define ASSERT(x, msg) if(!(x)) \
{\
  std::cerr << "[ASSERTION FAILED]: " << msg << std::endl;\
  abort();\
}
#else
#define ASSERT(x, msg)
#endif
