#include "ECSManager.h"

#include "Entity.h"
#include "System.h"

namespace ecs
{
  Entity* ECSManager::CreateEntity()
  {
    Entity* entity = new Entity(this);
    entities.push_back(entity);
    return entity;
  }

  void ECSManager::AddSystem(SystemBase* system)
  {
    systems.push_back(system);
  }

  void ECSManager::Update(float deltaTime)
  {
    for(auto&& system : systems)
    {
      system->Update(this, deltaTime);
    }
  }
}
