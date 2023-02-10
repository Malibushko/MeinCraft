#pragma once
#include "core/entity/Component.h"
#include <glm/mat4x4.hpp>

struct TTransformComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::mat4 Transform{1.f};
};