#pragma once

#include <vector>
#include <unordered_map>

#include "Config.h"

namespace ecs
{
  class EntitySet
  {
    private:
      std::vector<EntityID> entitiesList;
      std::unordered_map<EntityID, size_t> entitiesMap;
    public:
      bool Emplace(EntityID entity)
      {
        auto res = entitiesMap.emplace(entity, entitiesList.size());

        // Check if already exists
        if(!res.second)
          return false;

        entitiesList.push_back(entity);
        return true;
      }

      bool Erase(EntityID entity)
      {
        auto it = entitiesMap.find(entity);
        if(it == entitiesMap.end())
          return false;
        entitiesMap.erase(it);
        entitiesList.erase(entitiesList.begin() + it->second);
        return true;
      }

      bool Pop()
      {
        if(entitiesList.size() == 0)
          return false;
        entitiesMap.erase(entitiesMap.find(entitiesList.back()));
        entitiesList.pop_back();
        return true;
      }

      size_t Find(EntityID entity)
      {
        auto it = entitiesMap.find(entity);
        if(it == entitiesMap.end())
          return entitiesList.size();
        return it->second;
      }

      std::vector<EntityID>& GetList() { return entitiesList; }
      const std::vector<EntityID>& GetList() const { return entitiesList; }

      std::vector<EntityID>::iterator Begin() { return entitiesList.begin(); }
      std::vector<EntityID>::iterator End() { return entitiesList.end(); }

      std::vector<EntityID>::iterator begin() { return entitiesList.begin(); }
      std::vector<EntityID>::iterator end() { return entitiesList.end(); }
  };
};
