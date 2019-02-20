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

  struct ComponentContainerBase
  {
    ComponentContainerBase(){}
    virtual ~ComponentContainerBase(){}
  };

  template <typename Component>
  struct ComponentContainer : public ComponentContainerBase
  {
    Component component;
    ComponentContainer(Component&& component)
    : component{component}{}
    virtual ~ComponentContainer(){}
  };
}
