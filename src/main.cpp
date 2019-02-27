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
      //ASSERT((*it)->HasComponents<Collidable>(), "entity doesn't have specified component");
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
  entity->AddComponent<Movement>(1.0, 0.0);
  entity->AddComponent<Collidable>();
  entity->AddComponent<Nameable>("slim shady");

  Entity* dummy = manager->CreateEntity();
  dummy->AddComponent<Nameable>("Lil Pump");
  dummy->AddComponent<Transform>(10.0f, 2.0f, 2.0, 2.0);

  Entity* entity2 = manager->CreateEntity();
  entity2->AddComponent<Nameable>("Big shack");
  entity2->AddComponent<Transform>(10.0f, 2.0f, 2.0, 2.0);
  entity2->AddComponent<Movement>(0.0, 0.0);
  entity2->AddComponent<Collidable>();

  TransformDebugSystem* debugSystem = new TransformDebugSystem();

  manager->AddSystem(new MoveSystem());
  manager->AddSystem(new PhysicalSystem());
  manager->AddSystem(debugSystem);

  // Main loop
  for(int i = 0;i<10;i++)
  {
    std::cout << i << std::endl;
    manager->Update(1.0f);
    if(i == 1)
      manager->DestroyEntity(dummy);
    if(i == 3)
      manager->RemoveSystem(debugSystem);
    if(i == 8)
    {
      entity->RemoveComponent<Collidable>();
    }
  }
#endif

  ecs::ComponentContainer container = ecs::ComponentContainer(sizeof(Transform));
  for(float i = 0;i<10;i++)
  {
    container.Push(Transform(i,i));
  }
  container.ShrinkToFit();

  //container.Pop();
  container.Erase(3);
  container.Insert(3, Transform(3,3));
  for(auto comp : container)
  {
    Transform* t = (Transform*)comp;
    std::cout << t->x << " " << t->y << std::endl;
  }

  std::cout << container.Capacity() << std::endl;

  std::vector<int> vec;
  vec.begin();

  return 0;
}
