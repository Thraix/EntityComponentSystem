#include <iostream>

#include "ECSManager.h"

#include "ComponentSet.h"

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

struct Nameable
{
  std::string name;
  Nameable(const std::string& name)
    : name{name}
  {}
};

void MoveSystem(ecs::ECSManager* manager, float deltaTime)
{
  manager->Each<Transform, Movement>([deltaTime](EntityID entity, Transform& transform, Movement& movement)
  {
    transform.x += movement.vx * deltaTime;
    transform.y += movement.vy * deltaTime;
  });
}

void DebugSystem(ecs::ECSManager* manager)
{
  manager->Each<Nameable, Transform>([](EntityID entity, Nameable& nameable, Transform& transform)
  {
    std::cout << nameable.name << " is at location: " << transform.x << ", " << transform.y << std::endl;
  });
}

int main()
{
#if 1
  using namespace ecs;
  ECSManager* manager = new ECSManager();
  EntityID entity = manager->CreateEntity();
  manager->AddComponent<Transform>(entity, 1.1f, 2.0f);
  /* manager->AddComponent<Transform>(entity, 1.1f, 2.0f); */
  manager->AddComponent<Movement>(entity, 1.0f, 0.0f);
  manager->AddComponent<Collidable>(entity);
  manager->AddComponent<Nameable>(entity, "EntityID 1");

  EntityID dummy = manager->CreateEntity();
  manager->AddComponent<Nameable>(dummy, "EntityID 2");
  manager->AddComponent<Transform>(dummy, 10.0f, 2.0f, 2.0f, 2.0f);
  manager->AddComponent<Movement>(dummy, 10.0f, 2.0f);

  EntityID entity2 = manager->CreateEntity();
  manager->AddComponent<Nameable>(entity2, "EntityID 3");
  manager->AddComponent<Transform>(entity2, 10.0f, 2.0f, 2.0f, 2.0f);
  manager->AddComponent<Movement>(entity2, 0.0f, 0.0f);
  manager->AddComponent<Collidable>(entity2);
  manager->Each([](EntityID entity){
      std::cout << "EntityID: " << entity << std::endl;
      });
  manager->Each<Collidable>([](EntityID entity, Collidable& collidable){
      std::cout << "EntityID: " << entity << std::endl;
      });

  manager->Each<Transform, Movement, Collidable>([](EntityID entity, Transform& transform, Movement& movement, Collidable&){
      std::cout << "Each: " << entity << std::endl;
      });

  /* TransformDebugSystem* debugSystem = new TransformDebugSystem(); */

  /* manager->AddSystem(new MoveSystem()); */
  /* manager->AddSystem(new PhysicalSystem()); */
  /* manager->AddSystem(debugSystem); */

  // Main loop
  for(int i = 0;i<10;i++)
  {
    std::cout << i << std::endl;
    MoveSystem(manager, 1.0f);
    /* manager->Update(1.0f); */
    if(i == 1)
      manager->DestroyEntityID(dummy);
    if(i < 7)
      DebugSystem(manager);
    if(i == 8)
    {
      manager->RemoveComponent<Collidable>(entity);
    }
  }
#endif

  delete manager;
  return 0;
}
