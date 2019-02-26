#pragma once

#include <typeindex>

namespace ecs
{
  typedef std::type_index ComponentId;

  template <typename T>
  ComponentId GetComponentId()
  {
    return std::type_index(typeid(T));
  }
}
