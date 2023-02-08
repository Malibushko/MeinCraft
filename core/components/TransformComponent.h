#pragma once
#include "core/entity/Component.h"
#include <glm/vec4.hpp>

struct TTransformComponent : IComponent
{
  glm::vec4 Transform;
};