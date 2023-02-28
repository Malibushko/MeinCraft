#pragma once
#include <optional>
#include <glm/vec3.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Request.h"
#include "game/components/terrain/BlockComponent.h"

// Not set values will be calculated automatically
struct TBlockCreateRequest : TRequest
{
  // Request data
  EBlockType Type;
  glm::vec3  WorldPosition;

  // Service data
  entity_t BlockItem{entt::null};
};
