#pragma once
#include <optional>
#include <glm/vec3.hpp>

#include "core/entity/Entity.h"
#include "core/entity/Request.h"

// Not set values will be calculated automatically
struct TItemUseRequest : TOneshotRequest
{
  // Request data
  entity_t Item{entt::null};
};
