#pragma once

#include "Common.h"
#include "EntitySet.h"

#include <vector>


namespace ecs
{
  class ComponentPoolBase
  {
    protected:
      EntitySet entities;
    public:
      virtual ~ComponentPoolBase() = default;

      virtual size_t Size() = 0;
      virtual void Pop() = 0;
      virtual bool Erase(EntityID entity) = 0;
      std::vector<EntityID>& GetEntities() { return entities.GetList(); }
      const std::vector<EntityID>& GetEntities() const { return entities.GetList(); }
  };

  template <typename Component>
  class ComponentPool : public ComponentPoolBase
  {
    using Iterator = typename std::vector<Component>::iterator;
    private:
      std::vector<Component> components;

    public:
      ComponentPool(EntityID entity, const Component& component)
      {
        Emplace(entity, component);
      }

      Component& Emplace(EntityID entity, const Component& component)
      {
        components.push_back(component);
        entities.Emplace(entity);
        return components.back();
      }

      void Pop()
      {
        components.pop_back();
        entities.Pop();
      }

      bool Erase(EntityID entity)
      {
        if(!entities.Erase(entity))
          return false;
        size_t index = entities.Find(entity);
        components.erase(components.begin() + index);
        return true;
      }

      Component& At(size_t index)
      {
        return operator[](index);
      }

      size_t Find(EntityID entity)
      {
        return entities.Find(entity);
      }

      Component* FindComponent(EntityID entity)
      {
        size_t index = Find(entity);
        if(index < Size())
          return &components[index];
        return nullptr;
      }

      Component& operator[](size_t index)
      {
        ASSERT(index < components.size(), "Index Out of Bound Exception");
        return components[index];
      }

      size_t Size()
      {
        return components.size();
      }

      Iterator Begin()
      {
        return components.begin();
      }

      Iterator Back()
      {
        return components.back();
      }

      Iterator End()
      {
        return components.begin();
      }

      // Needed to do for each loops
      Iterator begin()
      {
        return components.begin();
      }

      Iterator end()
      {
        return components.end();
      }
  };
}
