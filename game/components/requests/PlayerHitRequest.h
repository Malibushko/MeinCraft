#pragma once
#include <optional>
#include <glm/vec3.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Request.h"

// Not set values will be calculated automatically
struct TPlayerHitRequest : TOneshotRequest
{
  // Request data
  glm::vec3 TargetPosition{};
  entity_t  Target{ entt::null };
};
