#pragma once

#ifdef _DEBUG
#define ASSERT(x, msg) if(!(x)) \
{\
  std::cerr << "ASSERTION FAILED IN " << __FILE__ << ":" <<__LINE__ << ": " << msg << std::endl;\
  abort();\
} 
#else
#define ASSERT(x, msg)
#endif
