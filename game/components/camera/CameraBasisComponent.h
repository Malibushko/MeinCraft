#pragma once
#include "GlobalTransformComponent.h"
#include "core/components/PositionComponent.h"

struct TCameraBasisComponent : IComponent
{
  glm::vec3 Up   { 0.0f, 1.0f, 0.0f };
  glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
  glm::vec3 Right{ 1.0f, 0.0f, 0.0f };

  float Yaw{};
  float Pitch{};
  float Roll{};
};
