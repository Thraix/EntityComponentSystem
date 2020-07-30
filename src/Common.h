#pragma once


#ifdef _DEBUG
#include <iostream>
#define ASSERT(x, msg) if(!(x)) \
{\
  std::cerr << "[ASSERTION FAILED]: " << msg << std::endl;\
  abort();\
}
#else
#define ASSERT(x, msg)
#endif
