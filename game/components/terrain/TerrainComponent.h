#pragma once
#include <map>
#include <glm/vec2.hpp>
#include <glm/gtx/hash.hpp>
#include "BlockComponent.h"
#include "ChunkComponent.h"
#include "core/entity/Component.h"
#include "core/entity/entity.h"

struct TTerrainComponent
{
  std::function<TBlockComponent(const glm::vec3 &)> TerrainGenerationStrategy;
  std::unordered_map<glm::ivec2, entity_t>          Chunks;


  entity_t GetChunkAt(glm::ivec2 ChunkCoordinates) const
  {
    if (auto Iterator = Chunks.find(ChunkCoordinates); Iterator != Chunks.end())
      return Iterator->second;

    return entt::null;
  }

  entity_t GetChunkAt(glm::vec3 WorldPosition) const
  {
    return GetChunkAt(ToChunkCoordinates(WorldPosition));
  }
};
