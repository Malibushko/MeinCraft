#pragma once
#include <optional>
#include <glm/vec3.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Request.h"

// Not set values will be calculated automatically
struct TRaycastRequest : TRequest
{
  // Request data
  std::optional<glm::vec3> StartPosition;
  std::optional<glm::vec3> Direction;
  float                    Distance;

  // Request result
  entity_t  RaycastHit{ entt::null };
  glm::vec3 EndPosition;
};
