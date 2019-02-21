#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
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
    std::unordered_map<ComponentId, void*> componentHandler; // void* is here a vector of the same component
    public:
      Entity* CreateEntity();

      template <typename Component, typename... Args>
      int CreateComponent(Entity* owner, Args... args)
      {
        auto it = componentHandler.find(GetComponentId<Component>());

        if(it != componentHandler.end())
        {
          auto vec = CastVector<Component>(it->second);
          vec->push_back(std::make_pair(owner, Component(args...)));
          return vec->size() - 1; 
        }
        else
        {
          std::vector<std::pair<Entity*, Component>>* vec = new std::vector<std::pair<Entity*, Component>>{};
          vec->push_back(std::make_pair(owner, Component(args...)));
          componentHandler.emplace(GetComponentId<Component>(), vec);
          return 0;
        }
      }

      template <typename Component>
      Component* GetComponent(int index)
      {
        auto it = componentHandler.find(GetComponentId<Component>());
        if(it != componentHandler.end())
        {
          auto vec = CastVector<Component>(it->second);
          return &vec->at(index).second;
        }
        return nullptr;
      }

      void AddSystem(SystemBase* system);

      template <typename... Components>
      EntityContainer<std::vector<Entity*>&, Components...> GetEntities()
      {
        return EntityContainer<decltype(entities)&,Components...>(entities);
      }

      void Update(float deltaTime);

    private:
      template <typename Component>
      inline std::vector<std::pair<Entity*, Component>>* CastVector(void* vec)
      {
        return static_cast<std::vector<std::pair<Entity*, Component>>*>(vec);
      }

  };
}
