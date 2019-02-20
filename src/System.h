#pragma once

#include <vector>

#include "Entity.h"
namespace ecs
{
  class SystemBase
  {
    friend class ECSManager;
    virtual void Update(ECSManager* manager, float deltaTime) = 0;
  };

  template <typename... Components>
  class System : public SystemBase
  {
    friend class ECSManager;
    void Update(ECSManager* manager, float deltaTime)  override = 0;
  };
}
