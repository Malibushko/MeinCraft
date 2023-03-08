#pragma once
#include <glm/vec3.hpp>

#include "core/entity/Component.h"

struct TLightComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::vec3 Color;
};