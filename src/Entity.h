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
    private:
      std::unordered_map<ComponentId, int> m_components;
      ECSManager* manager;
    public:
      friend class ECSManager;
      Entity(ECSManager* manager)
        : manager{manager}
      {
      }

      ~Entity()
      {
        //TODO: Tell the ECSManager to remove all my components
        for(auto component : m_components)
        {
          //delete component.second;
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
          m_components.emplace(GetComponentId<T>(), manager->CreateComponent<T>(this, args...));
        }
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
  };

  template <typename iterator, typename...Components>
  class EntityIterator
  {
    public:

      using difference_type = void;
      using value_type = Entity*;
      using pointer = void;
      using reference = Entity*;
      using iterator_category = std::bidirectional_iterator_tag;	

      iterator it;
      iterator last;

      EntityIterator(iterator it, iterator last)
        : it{it}, last{last}
      {
      }

      value_type operator*()
      {
        return *it;
      }

      EntityIterator<iterator, Components...>& operator++()
      {
        if(it != last)
          ++it;
        while(it != last && !(*it)->template HasComponents<Components...>())
        {
          ++it;
        }
        return *this;
      }

      EntityIterator<iterator, Components...> operator++(int)
      {
        auto tmp{*this};
        ++*this;
        return tmp;
      }

      bool operator==(EntityIterator<iterator, Components...> other)
      {
        return it == other.it;
      }

      bool operator!=(EntityIterator<iterator, Components...> other)
      {
        return it != other.it;
      }
  };

  template <typename Container, typename... Components>
  class EntityContainer
  {
    using iterator = decltype(std::begin(std::declval<Container>()));
    private:
      EntityIterator<iterator,Components...> first;
      EntityIterator<iterator,Components...> last;

    public:
      EntityContainer(Container& c)
        : first{std::begin(c), std::end(c)}, last{std::end(c), std::end(c)}
      {}

      EntityIterator<iterator,Components...> begin() { return first;}
      EntityIterator<iterator,Components...> end() { return last;}

  };

}
