#pragma once
#include <glm/vec4.hpp>

#include "core/entity/Component.h"

struct TLightComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::vec4 Color;
};