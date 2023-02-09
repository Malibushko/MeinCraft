#pragma once
#include <glm/mat4x4.hpp>

struct TGlobalTransformComponent
{
  static constexpr ComponentTag ComponentTag{};

  glm::mat4 Transform{ 1.0f };
};
