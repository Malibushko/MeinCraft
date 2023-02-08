#pragma once
#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"

struct TCameraComponent
{
  TTransformComponent Transform{};
  TPositionComponent  Position{};

  glm::vec3 Up   { 0.0f, 1.0f, 0.0f };
  glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
  glm::vec3 Right{ 1.0f, 0.0f, 0.0f };
};
