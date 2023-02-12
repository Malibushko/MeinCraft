#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct TDirectedLightComponent
{
  static constexpr ComponentTag ComponentTag{};

  float     Intensity = 1.f;
  glm::vec3 Direction;
};
