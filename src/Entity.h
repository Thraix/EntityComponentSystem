#pragma once

#include "Config.h"
#include "ECSManager.h"

namespace ecs
{
  class Entity
  {
    friend class ECSManager;

    private:
      ECSManager* manager;
      EntityID id;

      Entity(ECSManager* manager, EntityID id)
        : manager{manager}, id{id}
      {}

    public:

      operator EntityID() { return id; }
      operator bool() { return manager->ValidEntity(id); }

      void Destroy()
      {
        manager->DestroyEntity(id);
      }

      template <typename Component, typename... Args>
      inline Component& AddComponent(Args... args)
      {
        return manager->AddComponent<Component>(id, args...);
      }

      template <typename... Components>
      std::tuple<Components&...> AddComponents(Components&&... components)
      {
        return manager->AddComponents(id, components...);
      }

      template <typename Component>
      inline void RemoveComponent()
      {
        return manager->RemoveComponent<Component>(id);
      }

      template <typename... Components>
      inline void RemoveComponents()
      {
        return manager->RemoveComponents<Components...>(id);
      }

      template <typename Component>
      inline Component& GetComponent()
      {
        return manager->GetComponent<Component>(id);
      }

      template <typename Component>
      inline Component& HasComponent()
      {
        return manager->HasComponent<Component>(id);
      }

      static Entity Create(ECSManager* manager)
      {
        return {manager, manager->CreateEntity()};
      }
  };
}
