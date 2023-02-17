#pragma once
#include <glm/vec3.hpp>
#include "core/entity/Component.h"

struct TAABBVolumeComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::vec3 Min{};
  glm::vec3 Max{};
};
