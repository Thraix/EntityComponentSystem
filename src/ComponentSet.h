#pragma once

#include "Common.h"
#include "Config.h"
#include "EntitySet.h"

#include <vector>
#include <stdlib.h>
#include <cstring>
#include <utility>
#include <functional>
#include <exception>


namespace ecs
{

  class ComponentIterator
  {

    using difference_type = void;
    using value_type = void*;
    using pointer = void;
    using reference = void*;
    using iterator_category = std::bidirectional_iterator_tag;

    private:
      void* it;
      const void* start;
      const void* last;
      size_t byteSize;

    public:
      ComponentIterator(size_t byteSize, void* it, void* start, void* last)
        : it{it}, start{start}, last{last}, byteSize{byteSize}
      {
      }

      value_type operator*()
      {
        return it;
      }

      ComponentIterator& operator++()
      {
        if(it != last)
          it = (char*)it + byteSize;
        return *this;
      }

      ComponentIterator operator++(int)
      {
        auto tmp{*this};
        ++*this;
        return tmp;
      }

      ComponentIterator& operator--()
      {
        if(it != start)
          it = (char*)it - byteSize;
        return *this;
      }

      ComponentIterator operator--(int)
      {
        auto tmp{*this};
        --*this;
        return tmp;
      }

      bool operator==(ComponentIterator other)
      {
        return it == other.it;
      }

      bool operator!=(ComponentIterator other)
      {
        return it != other.it;
      }
  };

  class ComponentSet
  {
    using Iterator = ComponentIterator;
    private:
      size_t byteSize;
      size_t count;
      size_t reserve;
      void* memory;
      EntitySet entities;

    public:
      ComponentSet(size_t byteSize)
        : byteSize{byteSize}, reserve{1}, count{0}, memory{malloc(byteSize)}
      {
      }

      template <typename Component>
      ComponentSet(EntityID entity, Component&& component)
        : byteSize{sizeof(Component)}, reserve{1}, count{0}, memory{malloc(sizeof(Component))}
      {
        Emplace<Component>(entity, std::move(component));
      }

      ComponentSet(const ComponentSet& set) = delete;
      ComponentSet(ComponentSet&& set) = delete;

      ComponentSet& operator=(const ComponentSet& set) = delete;
      ComponentSet& operator=(ComponentSet&& set) = delete;

      ~ComponentSet()
      {
        free(memory);
      }

      template <typename Component>
      std::pair<bool, Iterator> Emplace(EntityID entity, Component&& component)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        if(!CheckResize())
          return {false, End()};

        Component* comp = ((Component*)memory)+count;
        *comp = component;
        count++;
        entities.Emplace(entity);
        return {true, Last()};
      }

      void Pop()
      {
        count--;
        entities.Pop();
      }

      bool Erase(EntityID entity)
      {
        size_t index = entities.Find(entity);
        if(!entities.Erase(entity))
          return false;
        memmove(
            (char*)memory + index*byteSize,
            (char*)memory + (index+1)*byteSize,
            (count - index - 1)*byteSize);
        count--;
        return true;
      }

      template<typename Component>
      Component* At(size_t index)
      {
        return (Component*)operator[](index);
      }

      size_t Find(EntityID entity)
      {
        return entities.Find(entity);
      }

      template <typename Component>
      Component* FindComponent(EntityID entity)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        size_t index = Find(entity);
        if(index < Size())
          return ((Component*)memory)+index;
        return nullptr;
      }

      void* operator[](size_t index)
      {
        ASSERT(index < count, "Index Out of Bound Exception");
        return (char*)memory + index*byteSize;
      }

      size_t Size()
      {
        return count;
      }

      size_t Capacity()
      {
        return reserve;
      }

      void Reserve(size_t reservate)
      {
        ASSERT(reservate >= count, "Trying to reserve less than the size of the container");
        void* newmem = malloc(reservate * byteSize);
        memcpy(newmem, memory, count * byteSize);
        free(memory);
        memory = newmem;
        reserve = reservate;
      }

      void ShrinkToFit()
      {
        Reserve(count);
      }

      Iterator Begin()
      {
        return Iterator(byteSize, memory, memory, (char*)memory+byteSize*count);
      }

      Iterator Last()
      {
        if(Size() == 0)
          return End();
        return Iterator(byteSize, (char*)memory+byteSize*count - byteSize, memory, (char*)memory+byteSize*count);
      }

      Iterator End()
      {
        return Iterator(byteSize, (char*)memory+byteSize*count, memory, (char*)memory+byteSize*count);
      }

      std::vector<EntityID>& GetEntities() { return entities.GetList(); }
      const std::vector<EntityID>& GetEntities() const { return entities.GetList(); }


      // Needed to do for each loops
      Iterator begin()
      {
        return Begin();
      }

      Iterator end()
      {
        return End();
      }

    private:
      bool CheckResize()
      {
        // Check if the buffer is full
        if(count == reserve)
        {
          // Full container
          if(reserve == (size_t)0 - (size_t)1)
          {
            return false;
          }
          // Last bit in reserve is 1 (almost full)
          else if((reserve & ((size_t)1 << (sizeof(size_t) * 8 - 1))) != 0)
          {
            reserve = (size_t)0 - (size_t)1;
          }
          else
          {
            // Double the size;
            reserve <<= 1;
            if(reserve == 0)
              reserve = 1;
            Reserve(reserve);
          }
        }
        return true;
      }
  };
}
