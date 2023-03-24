#pragma once
#include <glm/vec4.hpp>

#include "core/entity/Component.h"

struct TMaterialComponent
{
  static constexpr ComponentTag ComponentTag{};

  float     Metallic{};
  float     Roughness{};
  float     Emissive{};
  glm::vec4 EmissiveColor{};
};
