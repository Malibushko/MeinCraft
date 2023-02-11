#pragma once
#include <glm/mat4x4.hpp>
#include "core/entity/Component.h"

struct TGlobalTransformComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::mat4 Model{ glm::mat4(1.f) };
  glm::mat4 View{ glm::mat4(1.f) };
  glm::mat4 Projection{glm::mat4{1.f}};
};
