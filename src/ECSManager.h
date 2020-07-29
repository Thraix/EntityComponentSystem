#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "ComponentSet.h"
#include "Config.h"

namespace ecs
{
  class ECSManager final
  {
    private:
      bool destroyingEntityID;
      std::vector<EntityID> entities;
      std::unordered_map<std::type_index, ComponentSet*> componentPool;
    public:
      ~ECSManager()
      {
        for(auto&& components : componentPool)
        {
          delete components.second;
        }
        componentPool.clear();
      }

      EntityID CreateEntity()
      {
        static EntityID entityId = 1;
        entities.push_back(entityId);
        entityId++;
        return entityId-1;
      }

      void DestroyEntityID(EntityID entity)
      {
        auto it = std::find(entities.begin(), entities.end(), entity);
        ASSERT(it != entities.end(), "Entity does not exist in ECSManager");

        entities.erase(it);
        for(auto&& pools : componentPool)
        {
          size_t index = pools.second->Find(entity);
          if(index < pools.second->Size())
            pools.second->EraseIndex(entity);
        }
      }

      template <typename Component>
      Component* GetComponent(int index)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        if(it != componentPool.end())
        {
          return it->second->template At<Component>(index);
        }
        return nullptr;
      }

      template <typename Component, typename... Args>
      Component& AddComponent(EntityID entity, Args&&... args)
      {
        auto it = componentPool.find(GetComponentId<Component>());

        if(it != componentPool.end())
        {
          return *(Component*)*((it->second)->template Push<Component>(entity, Component(args...)).second);
        }
        else
        {
          auto ret = componentPool.emplace(GetComponentId<Component>(), new ComponentSet{entity, Component{args...}});
          return *((*ret.first).second->template At<Component>(0));
        }
      }

      template <typename Component>
      Component& AddComponent(EntityID entity, Component&& component)
      {
        auto it = componentPool.find(GetComponentId<Component>());

        if(it != componentPool.end())
        {
          return *(Component*)*((it->second)->template Push<Component>(entity, component).second);
        }
        else
        {
          auto ret = componentPool.emplace(GetComponentId<Component>(), new ComponentSet{entity, component});
          return *((*ret.first).second->template At<Component>(0));
        }
      }

      template <typename Component>
      void RemoveComponent(EntityID entity)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        ASSERT(it != componentPool.end(), "Component has not been added to an entity");
        it->second->EraseEntityID(entity);
      }

      template <typename Component>
      Component& GetComponent(EntityID entity)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        ASSERT(it != componentPool.end(), "Component has not been added to an entity");
        return *(Component*)*it->second->template FindComponent(entity);
      }

      template <typename Component>
      bool HasComponent(EntityID entity)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        if(it != componentPool.end())
          return it->second->Find(entity) != it->second->Size();
        return false;
      }

      template <typename... Components>
      bool HasComponents(EntityID entity)
      {
        return (HasComponent<Components>(entity) && ...);
      }

      template <typename Component, typename... Components, typename Func>
      void Each(Func function)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        if(it != componentPool.end())
        {
          size_t i = 0;
          for(auto entity : it->second->GetEntities())
          {
            if(HasComponents<Components...>(entity))
            {
              std::apply(function, std::forward_as_tuple(entity, *it->second->template At<Component>(i), GetComponent<Components>(entity)...));
            }
            i++;
          }
        }
      }

      template <typename Component>
      void Each(std::function<void(EntityID, Component&)> function)
      {
        auto it = componentPool.find(GetComponentId<Component>());
        if(it != componentPool.end())
        {
          size_t i = 0;
          for(auto e : it->second->GetEntities())
          {
            function(e, *it->second->template At<Component>(i));
            i++;
          }
        }
      }

      void Each(std::function<void(EntityID)> function)
      {
        for(auto e : entities)
          function(e);
      }

      template <typename T>
      std::type_index GetComponentId()
      {
        return std::type_index(typeid(T));
      }
  };
}
