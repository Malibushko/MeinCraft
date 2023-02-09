#pragma once
#include "core/entity/Component.h"
#include <glm/vec3.hpp>

struct TPositionComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::vec3 Position;
};
