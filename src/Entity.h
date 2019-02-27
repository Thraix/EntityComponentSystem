#pragma once

#include <unordered_map>
#include <iostream>
#include <functional>
#include "Component.h"
#include "ECSManager.h"

namespace ecs 
{

  class Entity
  {
    friend class ECSManager;
    private:
      std::unordered_map<ComponentId, int> m_components;
      ECSManager* manager;
      bool shouldDestroy;
    private:
      // Should be created by the ECSManager
      Entity(ECSManager* manager)
        : manager{manager}
      {
      }
    public:

      ~Entity()
      {
        if(!ShouldDestroy())
        {
          std::cerr << "-------------------------------------------------" << std::endl;
          std::cerr << "ERROR: Entity was not destroyed by the ECSManager" << std::endl;
          std::cerr << "-------------------------------------------------" << std::endl;
        }
      }

    public:

      template <typename T, typename U, typename... Args>
      bool HasComponents()
      {
        return HasComponents<T>() && HasComponents<U, Args...>();
      }

      template <typename T>
      bool HasComponents()
      {
        return m_components.find(GetComponentId<T>()) != m_components.end();
      }

      template <typename Component>
      Component* GetComponent()
      {
        auto it = m_components.find(GetComponentId<Component>());
        return it != m_components.end() ? manager->GetComponent<Component>(it->second) : nullptr; 
      }

      template <typename T, typename... Args>
      void AddComponent(Args&&... args)
      {
        if(m_components.find(GetComponentId<T>()) != m_components.end())
        {
          std::cerr << "-------------------------------" << std::endl;
          std::cerr << "ERROR: Component already exists" << std::endl;
          std::cerr << "-------------------------------" << std::endl;
          return;
        }
        else
        {
          m_components.emplace(GetComponentId<T>(), manager->CreateComponent<T>(args...));
        }
      }

      template <typename Component>
      void RemoveComponent()
      {
        auto it = m_components.find(GetComponentId<Component>());
        if(it == m_components.end())
        {
          std::cerr << "-------------------------------" << std::endl;
          std::cerr << "ERROR: Component doesn't exists" << std::endl;
          std::cerr << "-------------------------------" << std::endl;
          return;
        }
        manager->DestroyComponent<Component>(it->second);
        m_components.erase(it);
        ASSERT(m_components.find(GetComponentId<Component>()) == m_components.end(), "Failed to remove correct component");
      }


      template <typename Component>
      void UpdateComponentIndex(int index)
      {
        auto it = m_components.find(GetComponentId<Component>());
        if(it != m_components.end())
        {
          it->second = index;
        }
      }

      template <typename... Components>
      void With(std::function<void(Components...)> fWith)
      {
        fWith(GetComponent<Components>()...);
      }

      bool ShouldDestroy()
      {
        return shouldDestroy;
      }

    private:
      void PrepareDestroy()
      {
        shouldDestroy = true;
      }
  };
}
