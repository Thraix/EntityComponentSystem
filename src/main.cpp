#include <iostream>

#include "ECSManager.h"
#include "Entity.h"
#include "System.h"

#include "ComponentContainer.h"

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
  {

  }
};

struct MoveSystem : public ecs::System<Transform, Movement>
{
  void Update(ecs::ECSManager* manager, float deltaTime) override
  {
    // These are guaranteed to exist
    for(auto&& entity : GetEntities(manager)){
      Transform* transform = entity->GetComponent<Transform>();
      Movement* movement = entity->GetComponent<Movement>();
      transform->x += movement->vx * deltaTime;
      transform->y += movement->vy * deltaTime;
    }
  }
};

struct PhysicalSystem : public ecs::System<Transform, Collidable>
{
  void Update(ecs::ECSManager* manager, float deltaTime)
  {
    auto entities = GetEntities(manager);
    for(auto it = entities.begin(); it != entities.end(); ++it)
    {
      for(auto it2 = it; it2 != entities.end(); ++it2)
      {
        if(it2 == it)
          continue;
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

struct TransformDebugSystem : public ecs::System<Nameable, Transform>
{

  void Update(ecs::ECSManager* manager, float deltaTime)
  {
    for(auto&& entity : GetEntities(manager))
    {
      std::cout << entity->GetComponent<Nameable>()->name << " is at location: " << entity->GetComponent<Transform>()->x << ", " << entity->GetComponent<Transform>()->y << std::endl;
    }
  }
};


int main()
{
#if 1
  using namespace ecs;
  ECSManager* manager = new ECSManager();
  Entity* entity = manager->CreateEntity();
  entity->AddComponent<Transform>(1.1f, 2.0f);
  entity->AddComponent<Movement>(5.0, 0.0);
  entity->AddComponent<Collidable>();
  entity->AddComponent<Nameable>("slim shady");
  Entity* entity2 = manager->CreateEntity();
  entity2->AddComponent<Transform>(10.0f, 2.0f, 2.0, 2.0);
  entity2->AddComponent<Movement>(0.0, 0.0);
  entity2->AddComponent<Collidable>();
  entity2->AddComponent<Nameable>("Big shack");
  Entity* dummy = manager->CreateEntity();

  Transform* transform = entity->GetComponent<Transform>();
  manager->AddSystem(new MoveSystem());
  manager->AddSystem(new PhysicalSystem());
  manager->AddSystem(new TransformDebugSystem());

  // Main loop
  for(int i = 0;i<10;i++)
  {
    manager->Update(0.25f);
  }
#endif

  ecs::ComponentContainer container = ecs::ComponentContainer(sizeof(Transform));
  for(float i = 0;i<16;i++)
  {
    container.Push(Transform(i,i));
  }

  //container.Pop();
  container.Erase(3);
  container.Insert(3, Transform(3,3));
  for(size_t i = 0;i<container.Size();i++)
  {
    Transform* t = (Transform*)container[i];
    std::cout << t->x << " " << t->y << std::endl;
  }

  std::cout << container.ReserveSize() << std::endl;

  return 0;
}
