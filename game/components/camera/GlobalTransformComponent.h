#pragma once
#include <glm/mat4x4.hpp>

struct TGlobalTransformComponent : IComponent
{
  glm::mat4 Transform{ 1.0f };
};
