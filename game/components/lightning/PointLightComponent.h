#pragma once
#include <glm/vec3.hpp>

#include "core/entity/Component.h"

struct TPointLightComponent
{
  static constexpr ComponentTag ComponentTag{};

  float Constant;
  float Linear;
  float Quadratic;
};