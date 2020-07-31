#include <iostream>

#include "ECSManager.h"
#include "Entity.h"

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
{
  int shader;
  uint32_t color;
};

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

  Entity entity = Entity::Create(manager);
  entity.AddComponent<Nameable>("Entity 1");
  entity.AddComponent<Transform>(1.1f, 2.0f);
  entity.AddComponent<Movement>(1.0f, 0.0f);
  entity.AddComponent<Collidable>();

  Entity dummy = Entity::Create(manager);
  dummy.AddComponent<Nameable>("Entity 2");
  dummy.AddComponent<Transform>(10.0f, 2.0f, 2.0f, 2.0f);
  dummy.AddComponent<Movement>(10.0f, 2.0f);

  Entity entity2 = Entity::Create(manager);
  auto[name, trans, movement, collidable] = entity2.AddComponents<Nameable, Transform, Movement, Collidable>({"Entity 3"},{20.0f, 2.0f, 2.0f,2.0f}, {0.0f, 0.0f}, {});
  trans.x = 30;

  manager->Each([](EntityID entity){
      std::cout << "EntityID: " << entity << std::endl;
      });
  manager->Each<Collidable>([](EntityID entity, Collidable& collidable){
      std::cout << "EntityID: " << entity << std::endl;
      });

  manager->Each<Transform, Movement, Collidable>([](EntityID entity, Transform& transform, Movement& movement, Collidable&){
      std::cout << "Each: " << entity << std::endl;
      });

  // Main loop
  for(int i = 0;i<10;i++)
  {
    std::cout << "Iteration: " << i << std::endl;
    MoveSystem(manager, 1.0f);
    /* manager->Update(1.0f); */
    if(i == 1)
      manager->DestroyEntity(dummy);
    if(i < 7)
      DebugSystem(manager);
    if(i >= 8)
      if(manager->HasComponent<Collidable>(entity))
        manager->RemoveComponent<Collidable>(entity);
  }
#endif

  delete manager;
  return 0;
}
