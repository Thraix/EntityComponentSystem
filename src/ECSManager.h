#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include "Component.h"
#include "ComponentContainer.h"
#include "EntityContainer.h"

namespace ecs
{
  class Entity;
  class SystemBase;

  class ECSManager
  {
    bool destroyingEntity;
    std::vector<Entity*> entities;
    std::vector<SystemBase*> systems;
    std::unordered_map<ComponentId, ComponentContainer*> componentHandler;
    public:
      Entity* CreateEntity();
      void DestroyEntity(Entity* entity);

      template <typename Component, typename... Args>
      int CreateComponent(Args... args)
      {
        auto it = componentHandler.find(GetComponentId<Component>());

        if(it != componentHandler.end())
        {
          it->second->Push(Component(args...));
          return it->second->Size() - 1; 
        }
        else
        {
          ComponentContainer* container = new ComponentContainer(sizeof(Component));
          container->Push(Component(args...));
          componentHandler.emplace(GetComponentId<Component>(), container);
          return 0;
        }
      }

      template <typename Component>
      Component* GetComponent(int index)
      {
        auto it = componentHandler.find(GetComponentId<Component>());
        if(it != componentHandler.end())
        {
          return it->second->template At<Component>(index);
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
      void RemoveDestroyedEntities();

  };
}
