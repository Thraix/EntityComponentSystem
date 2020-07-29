#pragma once

#include "Common.h"
#include "Config.h"

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
      std::vector<EntityID> entities;

      // Keep track of the last found entity to speed up component getters
      std::pair<EntityID, size_t> findComponentCache = {0, 0};

    public:
      ComponentSet(size_t byteSize)
        : byteSize{byteSize}, reserve{1}, count{0}, memory{malloc(byteSize)}
      {
      }

      template <typename Component>
      ComponentSet(EntityID entity, Component&& component)
        : byteSize{sizeof(Component)}, reserve{1}, count{0}, memory{malloc(sizeof(Component))}
      {
        Push<Component>(entity, std::move(component));
      }

      ~ComponentSet()
      {
        free(memory);
      }

      template <typename Component>
      std::pair<bool, Iterator> Push(EntityID entity, Component&& component)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        if(!CheckResize())
          return {false, End()};

        Component* comp = ((Component*)memory)+count;
        *comp = component;
        count++;
        entities.push_back(entity);
        return {true, Last()};
      }

      void Pop()
      {
        count--;
      }

      template <typename Component>
      std::pair<bool, Iterator> Insert(size_t index, EntityID entity, Component&& component)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        ASSERT(index < count, "Index Out of Bound Exception");
        if(!CheckResize())
          return {false, End()};

        if(index == count)
          return Push(std::forward<Component>(component));

        memmove(
            (char*)memory + (index+1)*byteSize,
            (char*)memory + index*byteSize,
            (count - index)*byteSize);

        Component* comp = ((Component*)memory)+index;
        *comp = std::forward<Component>(component);
        count++;
        entities.insert(entities.begin() + index, entity);

        return {true, Last()};

      }

      void EraseEntityID(EntityID entity)
      {
        size_t index = Find(entity);
        ASSERT(index < Size(), "EntityID does not have component");
        EraseIndex(index);
      }

      void EraseIndex(size_t index)
      {
        ASSERT(index < count, "Index Out of Bound Exception");

        // Last element
        if(index == count - 1)
        {
          Pop();
          return;
        }

        memmove(
            (char*)memory + index*byteSize,
            (char*)memory + (index+1)*byteSize,
            (count - index - 1)*byteSize);
        entities.erase(entities.begin() + index);

        count--;
      }

      template<typename Component>
      Component* At(size_t index)
      {
        return (Component*)operator[](index);
      }

      size_t Find(EntityID entity)
      {
        /////////////////////
        // Check if the last found entity is the same
        if(findComponentCache.first == entity)
          return findComponentCache.second;

        int i = 0;
        for(auto e : entities)
        {
          if(e == entity)
          {
            findComponentCache = {e, i};
            return i;
          }
          i++;
        }
        return Size();
      }

      Iterator FindComponent(EntityID entity)
      {
        size_t index = Find(entity);
        if(index < Size())
          return Iterator(byteSize, (char*)memory+byteSize*index, memory, (char*)memory+byteSize*count);
        return End();
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


      // Needed to do for each loops
      Iterator begin()
      {
        return Begin();
      }

      Iterator end()
      {
        return End();
      }

      const std::vector<EntityID>& GetEntities() const
      {
        return entities;
      }

      std::vector<EntityID>& GetEntities()
      {
        return entities;
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
