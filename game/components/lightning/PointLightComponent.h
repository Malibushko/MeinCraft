#pragma once
#include <glm/vec3.hpp>

#include "core/entity/Component.h"

struct TPointLightComponent
{
  static constexpr ComponentTag ComponentTag{};

  float FadeConstant{};
  float FadeLinear{};
  float FadeQuadratic{};
  float Radius{};
};