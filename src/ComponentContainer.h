#pragma once

#include "Common.h"

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

  class ComponentContainer
  {
    using Iterator = ComponentIterator;
    private:
      size_t byteSize;
      size_t count;
      size_t reserve;
      void* memory;

    public:
      ComponentContainer(size_t byteSize, size_t n=0)
        : byteSize{byteSize}, reserve{n}, count{0}, memory{malloc(n)}
      {

      }

      ComponentContainer(const ComponentContainer&) = delete;

      ~ComponentContainer()
      {
        free(memory);
      }

      template <typename Component>
      bool Push(Component&& component)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        if(!CheckResize())
          return false;

        Component* comp = ((Component*)memory)+count;
        *comp = std::forward<Component>(component);
        count++;
        return true;
      }

      void Pop()
      {
        count--;
      }

      template <typename Component>
      bool Insert(size_t index, Component&& component)
      {
        ASSERT(sizeof(Component) == byteSize, "Size of Component doesn't match the byteSize");
        ASSERT(index < count, "Index Out of Bound Exception");
        if(!CheckResize())
          return false;

        if(index == count)
          return Push(std::forward<Component>(component));

        memmove(
            (char*)memory + (index+1)*byteSize, 
            (char*)memory + index*byteSize, 
            (count - index)*byteSize);

        Component* comp = ((Component*)memory)+index;
        *comp = std::forward<Component>(component);
        count++;

        return true;

      }

      void Erase(size_t index)
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

        count--;
      }

      template<typename Component>
      Component* At(size_t index)
      {
        return (Component*)operator[](index);
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
