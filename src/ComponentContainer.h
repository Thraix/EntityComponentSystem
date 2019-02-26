#pragma once

#include <stdlib.h>
#include <cstring>
#include <utility>
#include <exception>


// TODO
// Add an iterator class for this container
// Add Begin and End which returns iterators for this class
// Add function that will minimize the size of the container, currently
//   the size can only increase.
//
namespace ecs
{
  class ComponentContainer
  {
    size_t byteSize;
    size_t count;
    size_t reserve;
    void* memory;

    public:
      ComponentContainer(size_t byteSize, size_t n=0)
        : byteSize{byteSize}, reserve{n}, count{0}
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
        if(sizeof(Component) != byteSize)
          return false;
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
        if(sizeof(Component) != byteSize)
          return false;
        if(!CheckResize())
          return false;
        // Out of range
        if(index >= count)
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
        // Out of range
        if(index >= count)
          return;

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
        if(index >= count)
          return nullptr;
        return (char*)memory + index*byteSize;
      }

      size_t Size()
      {
        return count;
      }

      size_t ReserveSize()
      {
        return reserve;
      }

      bool Reserve(size_t reservate)
      {
        if(reservate < reserve)
          return false;
        void* newmem = malloc(reservate * byteSize); 
        memcpy(newmem, memory, count * byteSize);
        free(memory);
        memory = newmem;
        return true;
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
