#pragma once

#include <unordered_map>
#include <hash_map>
#include "Component.h"

namespace ecs
{
  class Entity;
  class SystemBase;

  template <typename container, typename... Components>
  class EntityContainer;


  class ECSManager
  {
    std::vector<Entity*> entities;
    std::vector<SystemBase*> systems;
    public:
      Entity* CreateEntity();
      void AddSystem(SystemBase* system);

      template <typename... Components>
      EntityContainer<std::vector<Entity*>&, Components...> GetEntities()
      {
        return EntityContainer<decltype(entities)&,Components...>(entities);
      }

      void Update(float deltaTime);
  };
}
