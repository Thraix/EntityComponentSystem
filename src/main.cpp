#include <iostream>

#include "ECSManager.h"
#include "Entity.h"
#include "System.h"


struct Transform
{
  float x;
  float y;
  float sx;
  float sy;
  Transform(float x, float y)
    : x{x}, y{y}, sx{1}, sy{1}
  {}
  Transform(float x, float y, float sx, float sy)
    : x{x}, y{y}, sx{sx}, sy{sy}
  {}
};

struct Renderable
{};

struct Movement 
{
  float vx;
  float vy;
  Movement()
    : vx{0}, vy{0}
  {}
  Movement(float vx, float vy)
    : vx{vx}, vy{vy}
  {}
};

struct Collidable
{};

struct MoveSystem : public ecs::System<Transform, Movement>
{
  void Update(ecs::ECSManager* manager, float deltaTime) override
  {
    // These are guaranteed to exist
    for(auto&& entity : manager->GetEntities<Transform,Movement>()){
      Transform* transform = entity->GetComponent<Transform>();
      Movement* movement = entity->GetComponent<Movement>();
      transform->x += movement->vx * deltaTime;
      transform->y += movement->vy * deltaTime;
    }
    //for(auto entity :manager->GetEntities<Transform, Movement>();
    //transform->x += movement->vx * deltaTime;
    //transform->y += movement->vy * deltaTime;
  }
};

struct PhysicalSystem : public ecs::System<Transform, Collidable>
{
  void Update(ecs::ECSManager* manager, float deltaTime)
  {
    auto entities = manager->GetEntities<Transform, Collidable>();
    for(auto it = entities.begin(); it != entities.end(); ++it)
    {
      for(auto it2 = it; it2 != entities.end(); ++it2)
      {
        if(it2 == it)
          continue;
        std::cout << "hello" << std::endl;
        CheckCollision(*it, *it2);
      }
    }
  }

  void CheckCollision(ecs::Entity* e1, ecs::Entity* e2)
  {
    Transform* t1 = e1->GetComponent<Transform>();
    Transform* t2 = e2->GetComponent<Transform>();
    
    if(t1->x < t2->x + t2->sx && t1->y < t2->y + t2->sy && t2->x < t1->x + t1->sx && t2->y < t1->y + t1->sy)
    {
      std::cout << "Collision has occured" << std::endl;
    }
  }
};



int main()
{
  using namespace ecs;
  ECSManager* manager = new ECSManager();
  Entity* entity = manager->CreateEntity();
  entity->AddComponent<Transform>(1.1f, 2.0f);
  entity->AddComponent<Movement>(5.0, 0.0);
  entity->AddComponent<Collidable>();
  Entity* entity2 = manager->CreateEntity();
  entity2->AddComponent<Transform>(10.0f, 2.0f);
  entity2->AddComponent<Movement>(0.0, 0.0);
  entity2->AddComponent<Collidable>();

  Transform* transform = entity->GetComponent<Transform>();
  manager->AddSystem(new MoveSystem());
  manager->AddSystem(new PhysicalSystem());

  // Main loop
  for(int i = 0;i<10;i++)
  {
    manager->Update(0.25f);
    std::cout << transform->x << " " << transform->y << std::endl;
  }

  return 0;
}