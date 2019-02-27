#include "ECSManager.h"

#include "Entity.h"
#include "EntityContainer.h"
#include "System.h"
#include "Common.h"

namespace ecs
{
  Entity* ECSManager::CreateEntity()
  {
    Entity* entity = new Entity(this);
    entities.push_back(entity);
    return entity;
  }

  void ECSManager::DestroyEntity(Entity* entity)
  {
    entity->PrepareDestroy();
    destroyingEntity = true;
  }

  void ECSManager::AddSystem(SystemBase* system)
  {
    systems.push_back(system);
  }

  SystemBase* ECSManager::RemoveSystem(SystemBase* system)
  {
    for(auto it = systems.begin(); it != systems.end(); ++it)
    {
      if(*it == system)
      {
        systems.erase(it);
        return system;
      }
    }
    return nullptr;
  }

  void ECSManager::Update(float deltaTime)
  {
    RemoveDestroyed();
    removeComponentQueue.clear();
    // Update the systems
    for(auto&& system : systems)
    {
      system->Update(this, deltaTime);
    }
  }

  void ECSManager::RemoveDestroyed()
  {
    std::unordered_map<ComponentId, std::set<int>> removedComponents;
    RemoveDestroyedEntities(removedComponents);
    RemoveDestroyedComponents(removedComponents);
    UpdateEntityComponentIndex(removedComponents);
  }

  void ECSManager::RemoveDestroyedEntities(std::unordered_map<ComponentId, std::set<int>>& removedComponents)
  {
    // Check whether an entity should be destroyed
    if(destroyingEntity)
    {
      for(auto itEntity = entities.begin();itEntity!=entities.end();)
      {
        // Check if this entity should be destroyed
        if((*itEntity)->ShouldDestroy())
        {
          // Remove all components that this entity owns
          for(auto&& component : (*itEntity)->m_components)
          {
            auto it = removedComponents.find(component.first);
            // Update the count of the removed component
            if(it == removedComponents.end())
              removedComponents.emplace(component.first, std::set<int>{component.second});
            else
              it->second.emplace(component.second);
          }
          delete *itEntity;
          itEntity = entities.erase(itEntity);
        }
        else
        {
          ++itEntity;
        }
      }
      destroyingEntity = false;
    }
  }

  void ECSManager::RemoveDestroyedComponents(std::unordered_map<ComponentId, std::set<int>>& removedComponents)
  {
    // Add all removed components to the map
    for(auto componentList : removeComponentQueue)
    {
      auto itComponentType = removedComponents.find(componentList.first);
      for(auto index : componentList.second)
      {
        if(itComponentType == removedComponents.end())
        {
          // Update iterator since it is now not at the end anymore.
          itComponentType = removedComponents.emplace(componentList.first, std::set<int>{index}).first;
        }
        else
        {
          // Add the index
          itComponentType->second.emplace(index);
        }
      }
    }

    // Go through all components that are going to get removed
    for(auto componentType : removedComponents)
    {
      ASSERT(
          componentHandler.find(componentType.first) != componentHandler.end(), 
          "Cannot find component type in ComponentHandler");
      // Get the container for the component
      auto container = componentHandler.find(componentType.first)->second;
      int count = 0;
      // Remove all components with that index
      for(auto componentIndex : componentType.second)
      {
        container->Erase(componentIndex-count);
        count++;
      }
    }
  }

  void ECSManager::UpdateEntityComponentIndex(std::unordered_map<ComponentId, std::set<int>>& removedComponents)
  {
    // go through all entities and update their indices 
    for(auto entity : entities)
    {
      for(auto removed : removedComponents)
      {
        auto it = entity->m_components.find(removed.first);
        // If the entity has this component update the index
        if(it != entity->m_components.end())
        {
          // Decrease the value by the amount of components that came before it that were removed. 
          size_t distance = std::distance(removed.second.begin(), removed.second.upper_bound(it->second));
          it->second -= distance;
        }
      }
    }
  }
}
