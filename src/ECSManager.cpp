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

  void ECSManager::Update(float deltaTime)
  {
    // Check whether an entity should be destroyed
    if(destroyingEntity)
    {
      std::unordered_map<ComponentId, int> removeCount;
      for(auto itEntity = entities.begin();itEntity!=entities.end();)
      {
        if((*itEntity)->ShouldDestroy())
        {
          for(auto&& component : (*itEntity)->m_components)
          {
            auto it = removeCount.find(component.first);
            if(it == removeCount.end())
              removeCount.emplace(component.first, 1);
            else
              it->second++;
            ASSERT(componentHandler.find(component.first) != componentHandler.end(), "Cannot find component type in ComponentHandler");
            componentHandler.find(component.first)->second->Erase(component.second);
          }
          delete *itEntity;
          itEntity = entities.erase(itEntity);
        }
        else
        {
          for(auto&& component : removeCount)
          {
            auto it = (*itEntity)->m_components.find(component.first);
            if(it != (*itEntity)->m_components.end())
            {
              it->second-=component.second;
            }
          }
          ++itEntity;
        }
      }
      destroyingEntity = false;
    }

    for(auto&& system : systems)
    {
      system->Update(this, deltaTime);
    }
  }
}
