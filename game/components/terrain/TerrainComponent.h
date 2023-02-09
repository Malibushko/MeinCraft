#pragma once
#include <map>
#include <glm/vec2.hpp>
#include <glm/gtx/hash.hpp>
#include "BlockComponent.h"
#include "core/entity/Component.h"
#include "core/entity/entity.h"

struct TTerrainComponent
{
  std::function<TBlockComponent(const glm::vec3 &)> TerrainGenerationStrategy;
  std::unordered_map<glm::ivec2, entity_t>          Chunks;
};
